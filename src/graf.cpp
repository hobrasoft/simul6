#include "graf.h"
#include "omp.h"
#include "pdebug.h"
#include "constituentseries.h"
#include "conductivityseries.h"
#include "phseries.h"
#include "electricfieldseries.h"
#include "grafdetail.h"
#include "msettings.h"
#include <iostream>
#include <cmath>
#include <math.h>
#include <QValueAxis>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include "grafstyle.h"
#include "manualscale.h"
#include "drawingflag.h"

#define PH_OFFSET -4
#define KAPA_OFFSET  -3
#define E_OFFSET  -2
#define DETECTOR_OFFSET -1


Graf::Graf(QWidget *parent) : GrafAbstract(parent)
{
    if (MSETTINGS->guiChartAntialiasing()) {
        setRenderHint(QPainter::Antialiasing);
        }
    setRubberBand(QChartView::RectangleRubberBand);
    m_chart = new QChart();
    setChart(m_chart);
    // Color: #AARRGGBB - AA = alpha channel, RR,GG,BB - red, gree, blue
    m_chart->setPlotAreaBackgroundPen(QPen(QColor("#50101010")));
    m_chart->setPlotAreaBackgroundVisible(true);
    m_rescaleIndividually = false;
    m_rescalePh = false;
    m_rescaleKapa = false;
    m_rescaleE = false;
    m_visiblePh = true;
    m_visibleKapa = true;
    m_visibleE = true;
    m_visibleDetector = false;
    m_engine = nullptr;
    m_axis_x = nullptr;
    m_axis_y = nullptr;
    m_detectorSeries = nullptr;
    m_detectorPosition = 0;
    m_drawing = false;

    m_actionRescale = new QAction(tr("Auto scale"), this);
    connect(m_actionRescale, &QAction::triggered, this, &Graf::autoscale);
    addAction(m_actionRescale);
    m_actionRescale->setEnabled(false);

    m_actionManualScale = new QAction(tr("Manual scale"), this);
    connect(m_actionManualScale, &QAction::triggered, this, &Graf::manualScale);
    addAction(m_actionManualScale);
    m_actionManualScale->setEnabled(false);

    #ifdef SET_AXIS_LABELS_MANUALLY
    m_actionSetAxisLabels = new QAction(tr("Adjust axis labels"), this);
    connect(m_actionSetAxisLabels, &QAction::triggered, this, &Graf::setAxisLabels);
    addAction(m_actionSetAxisLabels);
    m_actionSetAxisLabels->setEnabled(false);
    #endif

    m_actionSaveImage = new QAction(tr("Save chart to image"), this);
    connect(m_actionSaveImage, &QAction::triggered, this, &Graf::saveImage);
    addAction(m_actionSaveImage);
    m_actionSaveImage->setEnabled(false);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    m_rescaleEnabled = true;
    setMouseTracking(true);

}


void Graf::init(const Engine *pEngine) {
    if (pEngine == nullptr) { return; }
    m_chart->removeAllSeries();

    m_engine = QPointer<const Engine>(pEngine);
    pEngine->lock();
    size_t p = pEngine->getNp(); // points
    double inc_x = pEngine->getCapLen() / p;
    const Mix& mix = m_engine->getMix();
    auto kapa = pEngine->getKapa();

    int id = 0;
    for (auto &sample : mix.getSamples()) {
        ConstituentSeries *series = new ConstituentSeries(sample, this);
        connect(series, &ConstituentSeries::clicked, this, &Graf::seriesClicked);

        double x = 0;
        for (unsigned int i = 0; i <= p; i++){
            series->append(QPointF(x * 1000.0, value(sample, i, kapa[i]))); // sample.getA(0, i)));
            x += inc_x;
            }
        id += 1;

        m_chart->addSeries(series);
        }


    ConstituentSeries *series = new PhSeries(this);
    connect(series, &ConstituentSeries::clicked, this, &Graf::seriesClicked);
    series->setVisible(m_visiblePh);
    double x = 0;
    auto hpl = pEngine->getHpl();
    for (unsigned int i = 0; i <= p; i++){
        if (hpl[i] > 0) {
            double pH = -log(hpl[i]) / log(10);
            series->append(QPointF(x * 1000.0, pH));
            }
        x += inc_x;
        }
    m_chart->addSeries(series);

    series = new ConductivitySeries(this);
    connect(series, &ConstituentSeries::clicked, this, &Graf::seriesClicked);
    series->setVisible(m_visibleKapa);
    x = 0;
    for (unsigned int i = 0; i <= p; i++){
        series->append(QPointF(x * 1000.0, kapa[i]*100.0));
        x += inc_x;
        }
    m_chart->addSeries(series);

    series = new ElectricFieldSeries(this);
    connect(series, &ConstituentSeries::clicked, this, &Graf::seriesClicked);
    series->setVisible(m_visibleE);
    x = 0;
    auto efield = pEngine->getE();
    for (unsigned int i = 0; i <= p; i++){
        series->append(QPointF(x * 1000.0, fabs(efield[i]/1000.0)));
        x += inc_x;
        }
    m_chart->addSeries(series);

    pEngine->unlock();
    m_chart->legend()->setVisible(false);

    setAxisLabels();

    QBrush detectorBrush(QColor(DETECTOR_COLOR));
    QPen   detectorPen(detectorBrush, PENWIDTH);

    m_detectorSeries = new QLineSeries(this);
    m_detectorSeries->append(QPointF(-10, -99999));
    m_detectorSeries->append(QPointF(-10, +99999));
    m_detectorSeries->setUseOpenGL(true);
    m_detectorSeries->setVisible(m_visibleDetector);
    m_detectorSeries->setPen(detectorPen);
    m_detectorSeries->setBrush(detectorBrush);
    m_chart->addSeries(m_detectorSeries);
    setDetectorPosition(m_detectorPosition);

}


void Graf::setDetectorPosition(double x) {
    m_detectorPosition = x;
    if (m_detectorSeries == nullptr) { return; }
    m_detectorSeries->clear();
    m_detectorSeries->append(QPointF(x, -99999));
    m_detectorSeries->append(QPointF(x, +99999));
}


void Graf::swap() {
    QRectF rect;
    rect.setRight  (m_axis_x->max());
    rect.setLeft   (m_axis_x->min());
    rect.setBottom (m_axis_y->min());
    rect.setTop    (m_axis_y->max());
    setScale(rect.normalized());
}


void Graf::autoscale() {
    // PDEBUG;
    if (m_engine == nullptr) { return; }
    double maximum = -9999999;
    double minimum =  9999999;
    m_engine->lock();
    size_t p        = m_engine->getNp();
    auto hpl        = m_engine->getHpl();
    const Mix& mix  = m_engine->getMix();
    auto kapa       = m_engine->getKapa();
    auto efield     = m_engine->getE();
    double mcaplen = 1000 * m_engine->getCapLen();

    unsigned int xleft = 0;
    unsigned int xright = p;
    if (m_rescaleIndividually && m_axis_x != nullptr) {
        double d_xleft  = m_axis_x->min();
        double d_xright = m_axis_x->max();
        xleft = p * d_xleft /mcaplen;
        xright = p * d_xright /mcaplen;
        }

    for (auto &sample : mix.getSamples()) {
        if (!sample.visible()) { continue; }
        if (m_rescaleIndividually && sample.getInternalId() != m_rescaleId) { continue; }
        for (unsigned int i = xleft; i <= xright; i++){
            double val = value(sample, i, kapa[i]);
            if (val > maximum)  {
                maximum = val;
                }
            if (val < minimum)  {
                minimum = val;
                }
            }
        }
    m_engine->unlock();

    bool rescalePh = (m_rescaleIndividually && m_rescalePh && m_visiblePh) ||
                     (!m_rescaleIndividually && m_visiblePh);
    for (unsigned int i = xleft; rescalePh && i <= xright; i++) {
        if (hpl[i] > 0) {
            double pH = -log(hpl[i]) / log(10);
            if (pH > maximum) { 
                maximum = pH;
                }
            if (pH < minimum) { 
                minimum = pH;
                }
            }
        }

    bool rescaleKapa = (m_rescaleIndividually && m_rescaleKapa && m_visibleKapa) ||
                       (!m_rescaleIndividually && m_visibleKapa);
    for (unsigned int i = xleft; rescaleKapa && i <= xright; i++) {
        if (kapa[i] * 100 > maximum) {
            maximum = kapa[i] * 100.0;
            }
        if (kapa[i] * 100 < minimum) {
            minimum = kapa[i] * 100.0;
            }
        }

    bool rescaleE = (m_rescaleIndividually && m_rescaleE && m_visibleE) ||
                    (!m_rescaleIndividually && m_visibleE);
    for (unsigned int i = xleft; rescaleE && i <= xright; i++) {
        if (fabs(efield[i] / 1000.0) > maximum) {
            maximum = fabs(efield[i] / 1000.0);
            }
        if (fabs(efield[i] / 1000.0) < minimum) {
            minimum = fabs(efield[i] / 1000.0);
            }
        }

    QRectF rect;
    rect.setTop    (minimum - 0.09 * (maximum-minimum) );
    rect.setBottom (maximum + 0.09 * (maximum-minimum) );
    rect.setLeft   (0);
    rect.setRight  (mcaplen);
    setScale(rect.normalized());

    if (m_rescaleIndividually && m_axis_x != nullptr) {
        double d_xleft  = m_axis_x->min();
        double d_xright = m_axis_x->max();
        rect.setLeft (d_xleft);
        rect.setRight(d_xright);
        }
    repaint();
}


void Graf::rescalePh() {
    m_rescaleIndividually = true;
    m_rescalePh = true;
    autoscale();
    m_rescalePh = false;
    m_rescaleIndividually = false;
}


void Graf::rescaleE() {
    m_rescaleIndividually = true;
    m_rescaleE = true;
    autoscale();
    m_rescaleE = false;
    m_rescaleIndividually = false;
}


void Graf::rescaleKapa() {
    m_rescaleIndividually = true;
    m_rescaleKapa = true;
    autoscale();
    m_rescaleKapa = false;
    m_rescaleIndividually = false;
}


void Graf::rescale(int internalId) {
    m_rescaleIndividually = true;
    m_rescaleId = internalId;
    autoscale();
    m_rescaleId = 0;
    m_rescaleIndividually = false;
}


void Graf::manualScale() {
    double caplen = 100;
    if (m_axis_y == nullptr || m_axis_y == nullptr) {
        return;
        }
    if (m_engine != nullptr) {
        m_engine->lock();
        caplen =  m_engine->getCapLen() * 1000;
        m_engine->unlock();
        }

    QRectF rect;
    rect.setBottom(m_axis_y->min());
    rect.setTop(m_axis_y->max());
    rect.setLeft(m_axis_x->min());
    rect.setRight(m_axis_x->max());

    ManualScale *d = new ManualScale(this);
    d->setRect(rect);
    d->setCaplen(caplen);
    if (d->exec() == QDialog::Accepted) {
        setScale(d->rect().normalized());
        }
    d->deleteLater();
}


void Graf::setAxisLabels() {
    if (m_axis_y == nullptr || m_axis_x == nullptr) { 
        return;
        }
    QPointF topleft(m_axis_x->min(), m_axis_y->min());
    QPointF rightbottom(m_axis_x->max(), m_axis_y->max());
    QRectF rect(topleft, rightbottom);
    setScale(rect.normalized());
}


// prevents the unlimited scale
void Graf::mousePressEvent(QMouseEvent *event) {
    m_pressedPoint = event->pos();
    if (event->button() != Qt::LeftButton) {
        QChartView::mousePressEvent(event);
        return;
        }

    QRectF  rect        = m_chart->geometry(); // m_chart->plotArea();
    QPointF topLeft     = m_chart->mapToValue(rect.topLeft());
    QPointF bottomRight = m_chart->mapToValue(rect.bottomRight());
    QRectF rext(topLeft, bottomRight);
    rext = rext.normalized();

    if (rext.width() < 10e-12 || rext.height() < 10e-12) {
        m_pressedPoint = QPoint(-1,-1);
        event->accept();
        QChartView::mousePressEvent(event);
        return;
        }

    QChartView::mousePressEvent(event);
}


// prevents the context menu 
void Graf::mouseReleaseEvent(QMouseEvent *event) { 
    if (event->button() == Qt::RightButton) { 
        event->accept();
        // QChartView::mouseReleaseEvent(event);
        return;
        }

    if (m_pressedPoint.x() == -1 && m_pressedPoint.y() == -1) {
        event->accept();
        QChartView::mouseReleaseEvent(event);
        return;
        }

    if (std::pow((m_pressedPoint.x() - event->x()), 2) +
        std::pow((m_pressedPoint.y() - event->y()), 2) < 9) {
        event->accept();
        QChartView::mouseReleaseEvent(event);
        return;
        }

    QChartView::mouseReleaseEvent(event);
    subselected();
}


void Graf::subselected() {
    if (m_axis_x == nullptr) { return; }
    if (m_axis_y == nullptr) { return; }

    QRectF  rect        = m_chart->geometry(); // m_chart->plotArea();

    QPointF topLeft     = m_chart->mapToValue(rect.topLeft());
    QPointF bottomRight = m_chart->mapToValue(rect.bottomRight());
    QRectF rext(topLeft, bottomRight);

    // PDEBUG << rect << rext.normalized() << m_chart->plotArea();

    setScale(rext.normalized());

}


void Graf::setScale(const QRectF& rect) {
    if (rect.isNull()) {
        return;
        }
    if (rect.height() <= 0 || rect.width() <= 0) {
        // PDEBUG << "Invalid rect dimensions" << rect << rect.height() << rect.width();
        return;
        }
    if (m_engine == nullptr) { return; }

    m_actionManualScale->setEnabled(true);
    m_actionRescale->setEnabled(true);
    #ifdef SET_AXIS_LABELS_MANUALLY
    m_actionSetAxisLabels->setEnabled(true);
    #endif

    QList<QAbstractSeries *> serieslist = m_chart->series();
    for (int i=0; i<serieslist.size(); i++) {
        QList<QAbstractAxis *> axes = serieslist[i]->attachedAxes();
        for (int a=0; a<axes.size(); a++) {
            serieslist[i]->detachAxis(axes[a]);
            }
        }
    QList<QAbstractAxis *> axislistX = m_chart->axes(Qt::Horizontal);
    for (int i=0; i<axislistX.size(); i++) {
        m_chart->removeAxis(axislistX[i]);
        }
    QList<QAbstractAxis *> axislistY = m_chart->axes(Qt::Vertical);
    for (int i=0; i<axislistY.size(); i++) {
        m_chart->removeAxis(axislistY[i]);
        }

    if (m_axis_x != nullptr) { m_axis_x->deleteLater(); }
    if (m_axis_y != nullptr) { m_axis_y->deleteLater(); }

    m_axis_y = new QValueAxis(this);
    m_axis_x = new QValueAxis(this);
    m_axis_y->setRange(rect.top(), rect.bottom());
    m_axis_x->setRange(rect.left(), rect.right());
    m_axis_x->setTitleText(tr("mm"));

    #if QT_VERSION > 0x050c00
    // Not supported in Qt < 5.12
    double ix = axisTable(rect.width());
    double iy = axisTable(rect.height());
    m_axis_x->setTickInterval(ix);
    m_axis_y->setTickInterval(iy);

    double anchor_x = (rect.left() <= 0) ? 0 : pow(10, round(log10(rect.left())) );
    double anchor_y = (rect.bottom() <= 0) ? 0 : pow(10, round(log10(rect.bottom())) );
    m_axis_x->setTickAnchor(anchor_x);
    m_axis_y->setTickAnchor(anchor_y);
    m_axis_x->setTickType(QValueAxis::TicksDynamic);
    m_axis_y->setTickType(QValueAxis::TicksDynamic);
    #endif

    m_chart->addAxis(m_axis_x, Qt::AlignBottom);
    m_chart->addAxis(m_axis_y, Qt::AlignLeft);
    for (int i=0; i<serieslist.size(); i++) {
        serieslist[i]->attachAxis(m_axis_y);
        serieslist[i]->attachAxis(m_axis_x);
        }

}


double Graf::axisTable(double maximum) {
    maximum = maximum * 0.8;
    return
          (maximum <= 0.000000000000010) ? 0.0000000000000020
        : (maximum <= 0.000000000000020) ? 0.0000000000000050
        : (maximum <= 0.000000000000050) ? 0.0000000000000100

        : (maximum <= 0.00000000000010) ? 0.000000000000020
        : (maximum <= 0.00000000000020) ? 0.000000000000050
        : (maximum <= 0.00000000000050) ? 0.000000000000100

        : (maximum <= 0.0000000000010) ? 0.00000000000020
        : (maximum <= 0.0000000000020) ? 0.00000000000050
        : (maximum <= 0.0000000000050) ? 0.00000000000100

        : (maximum <= 0.000000000010) ? 0.0000000000020
        : (maximum <= 0.000000000020) ? 0.0000000000050
        : (maximum <= 0.000000000050) ? 0.0000000000100

        : (maximum <= 0.00000000010) ? 0.000000000020
        : (maximum <= 0.00000000020) ? 0.000000000050
        : (maximum <= 0.00000000050) ? 0.000000000100

        : (maximum <= 0.0000000010) ? 0.00000000020
        : (maximum <= 0.0000000020) ? 0.00000000050
        : (maximum <= 0.0000000050) ? 0.00000000100

        : (maximum <= 0.000000010) ? 0.0000000020
        : (maximum <= 0.000000020) ? 0.0000000050
        : (maximum <= 0.000000050) ? 0.0000000100

        : (maximum <= 0.00000010) ? 0.000000020
        : (maximum <= 0.00000020) ? 0.000000050
        : (maximum <= 0.00000050) ? 0.000000100

        : (maximum <= 0.0000010) ? 0.00000020
        : (maximum <= 0.0000020) ? 0.00000050
        : (maximum <= 0.0000050) ? 0.00000100

        : (maximum <= 0.000010) ? 0.0000020
        : (maximum <= 0.000020) ? 0.0000050
        : (maximum <= 0.000050) ? 0.0000100

        : (maximum <= 0.00010) ? 0.000020
        : (maximum <= 0.00020) ? 0.000050
        : (maximum <= 0.00050) ? 0.000100

        : (maximum <= 0.0010) ? 0.00020
        : (maximum <= 0.0020) ? 0.00050
        : (maximum <= 0.0050) ? 0.00100

        : (maximum <= 0.010) ? 0.0020
        : (maximum <= 0.020) ? 0.0050
        : (maximum <= 0.050) ? 0.0100

        : (maximum <= 0.10) ? 0.020
        : (maximum <= 0.20) ? 0.050
        : (maximum <= 0.50) ? 0.100

        : (maximum <= 1.) ? 0.20
        : (maximum <= 2.) ? 0.50
        : (maximum <= 5.) ? 1.0

        : (maximum <= 10.) ? 2.0
        : (maximum <= 20.) ? 5.0  
        : (maximum <= 50.) ? 10.0  

        : (maximum <= 100.) ? 20.0
        : (maximum <= 200.) ? 50.0  
        : (maximum <= 500.) ? 100.0  
        : (maximum <= 1000.) ? 200.0  

        : 500;
}


void Graf::setVisible(int id, bool visible) {
    QList<QAbstractSeries *> list = m_chart->series();
    for (int i=0; i<list.size(); i++) {
        ConstituentSeries *series = qobject_cast<ConstituentSeries *>(m_chart->series()[i]);
        if (series == nullptr) { continue; }
        if (series->internalId() == id) {
            series->setVisible(visible);
            // autoscale();
            return;
            }
        }
}


void Graf::setVisibleDetector(bool visible) {
    m_visibleDetector = visible;
    QList<QAbstractSeries *> list = m_chart->series();
    int i = list.size()+DETECTOR_OFFSET;
    if (i<0) { return; }
    list[i]->setVisible(visible);
}


void Graf::setVisiblePh(bool visible) {
    m_visiblePh = visible;
    QList<QAbstractSeries *> list = m_chart->series();
    int i = list.size()+PH_OFFSET;
    if (i<0) { return; }
    list[i]->setVisible(visible);
    // autoscale();
}


void Graf::setVisibleKapa(bool visible) {
    m_visibleKapa = visible;
    QList<QAbstractSeries *> list = m_chart->series();
    int i = list.size()+KAPA_OFFSET;
    if (i<0) { return; }
    list[i]->setVisible(visible);
    // autoscale();
}


void Graf::setVisibleE(bool visible) {
    m_visibleE = visible;
    QList<QAbstractSeries *> list = m_chart->series();
    int i = list.size()+E_OFFSET;
    if (i<0) { return; }
    list[i]->setVisible(visible);
    // autoscale();
}


void Graf::showGlobalActions(bool x) {
    m_actionRescale->setVisible(x);
    m_actionManualScale->setVisible(x);
    m_actionSaveImage->setVisible(x);
}


void Graf::slotRun() {
    m_actionSaveImage->setEnabled(false);
}


void Graf::slotFinished() {
    m_actionSaveImage->setEnabled(true);
}


double Graf::value(const Sample& sample, int i, double kapa) {
    Q_UNUSED(kapa);
    return sample.getA(0, i);
}


void Graf::drawGraph(const Engine *pEngine)
{
    if (m_drawing) { return; }
    DrawingFlag flag(&m_drawing);
    size_t p = pEngine->getNp(); // points
    const Mix& mix = pEngine->getMix();
    QLineSeries *series;
    auto kapa = pEngine->getKapa();

    int id = 0;
    double inc_x = pEngine->getCapLen() / p;

    for (auto &sample : mix.getSamples()) {
        series = qobject_cast<QLineSeries *>(m_chart->series()[id]);
        if (series == nullptr) { continue; }
        // PDEBUG << sample.getName() << sample.getInternalId();
        double x = 0;
        QList<double> vlist;
        QVector<QPointF> plist;
        for (unsigned int i = 0; i <= p; i++){
            plist << QPointF(x * 1000.0, value(sample, i, kapa[i]));
            vlist << x;
            x += inc_x;
            }
        series->replace(plist);
        series->setVisible(sample.visible());
        id += 1;
        }

/*
    // drawing mju over kapa
    for (auto &sample : mix.getSamples()) {
        series = qobject_cast<QLineSeries *>(m_chart->series()[id]);
        if (series == nullptr) { continue; }
        double x = 0;
        QList<double> vlist;
        QVector<QPointF> plist;
        for (unsigned int i = 0; i <= p; i++){
            double mjuoverkapa = 0;
            for (int j = sample.getNegCharge(); j <= sample.getPosCharge(); j++) {
                if (j == 0) { continue; }
                mjuoverkapa = mjuoverkapa + sample.getA(j, i) * sample.getU(j, i) * j / abs(j); // (j>0) ? 1 : (j<0) ? -1 : 0;
//                PDEBUG << i << j << sample.getU(j,i) << sample.getA(j,i) << mjuoverkapa;
            }
            if (sample.getA(0, i) > 0.001) mjuoverkapa = mjuoverkapa / sample.getA(0, i) / kapa[i]; else mjuoverkapa = 0;
            plist << QPointF(x * 1000.0, mjuoverkapa * 1E7);
            x += inc_x;
            }
        series->replace(plist);
        series->setVisible(sample.visible());
        id += 1;
        }
*/

//    int id = 8;
    series = qobject_cast<QLineSeries *>(m_chart->series()[id]);
    double x = 0;
    auto hpl = pEngine->getHpl();
    QVector<QPointF> plist;
    for (unsigned int i = 0; i <= p; i++) {
        double pH = (hpl[i] > 0) ? (-log(hpl[i]) / log(10)) : 0;
        plist << QPointF(x * 1000.0, pH);
        x += inc_x;
        }
    series->replace(plist);
    id += 1;

    x = 0;
    series = qobject_cast<QLineSeries *>(m_chart->series()[id]);
    plist.clear();;
    for (unsigned int i = 0; i <= p; i++) {
        plist << QPointF(x * 1000.0, kapa[i]*100.0);
        x += inc_x;
        }
    series->replace(plist);
    id += 1;

    x = 0;
    series = qobject_cast<QLineSeries *>(m_chart->series()[id]);
    auto efield = pEngine->getE();
    plist.clear();;
    for (unsigned int i = 0; i <= p; i++) {
        plist << QPointF(x * 1000.0, fabs(efield[i]/1000.0));
        x += inc_x;
        }
    series->replace(plist);
    id += 1;

}


void Graf::seriesClicked(const QPointF& point) {
    // PDEBUG;
    QLineSeries *s1 = qobject_cast<QLineSeries *>(sender());
    if (s1 == nullptr) { return; }
    if (m_engine == nullptr) { return; }

    m_engine->lock();
    QPoint position = mapToGlobal(QPoint(15,15));
    double caplen   = m_engine->getCapLen() * 1000.0;
    int    np       = m_engine->getNp();
    int    node     = (np+1) * point.x() / caplen;
    double x        = ((double)node) * (caplen / ((double)np));
    double y        = point.y();
    auto   hpl      = m_engine->getHpl();
    const Mix& mix  = m_engine->getMix();
    auto   kapa     = m_engine->getKapa();
    auto   efield   = m_engine->getE();

    ConstituentSeries *s2 = qobject_cast<ConstituentSeries *>(s1);
    if (s2 != nullptr && s2->internalId() != 0) {
        double minimumd = 1e99;
        double minimumy = 1e99;
        for (auto &sample : mix.getSamples()) {
            double sample_y = value(sample, node, kapa[node]); // sample.getA(0, node);
            double distance = fabs(y - sample_y);
            if (distance < minimumd) {
                minimumd = distance;
                minimumy = sample_y;
                }
            }
        m_engine->unlock();
        // PDEBUG << s2->name() << s2->internalId();
        GrafDetail *d = new GrafDetail(this, s2->name(), "mM", x, minimumy, node);
        d->move(position);
        d->show();
        connect(d, &QObject::destroyed, s2, &ConstituentSeries::setNormalWidth);
        return;
        }
    m_engine->unlock();

    int seriescount = m_chart->series().size();
    // PDEBUG << seriescount << s1 << m_chart->series()[seriescount+PH_OFFSET] << "PH";
    // PDEBUG << seriescount << s1 << m_chart->series()[seriescount+KAPA_OFFSET] << "KAPA";
    // PDEBUG << seriescount << s1 << m_chart->series()[seriescount+E_OFFSET] << "E";
    if (s1 == m_chart->series()[seriescount+PH_OFFSET]) {
        double pH = -log(hpl[node]) / log(10);
        GrafDetail *d = new GrafDetail(this, tr("pH"), "", x, pH, node);
        d->move(position);
        d->show();
        connect(d, &QObject::destroyed, s2, &ConstituentSeries::setNormalWidth);
        return;
        }

    if (s1 == m_chart->series()[seriescount+KAPA_OFFSET]) {
        double k = kapa[node] * 1000.0;
        GrafDetail *d = new GrafDetail(this, tr("Conductivity"), "mS/m", x, k, node);
        d->move(position);
        d->show();
        connect(d, &QObject::destroyed, s2, &ConstituentSeries::setNormalWidth);
        return;
        }

    if (s1 == m_chart->series()[seriescount+E_OFFSET]) {
        double e = efield[node];
        GrafDetail *d = new GrafDetail(this, tr("Electric field"), "V/m", x, e, node);
        d->move(position);
        d->show();
        connect(d, &QObject::destroyed, s2, &ConstituentSeries::setNormalWidth);
        return;
        }

}


void Graf::saveImage() {
    PDEBUG;
    QString dirname = MSETTINGS->exportDirName();
    QString filename = QFileDialog::getSaveFileName(this, tr("Save chart data"), dirname, tr("PNG format (*.png)")).trimmed();
    if (filename.isEmpty()) { return; }
    MSETTINGS->setExportDirName(QFileInfo(filename).absoluteDir().absolutePath());

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        PDEBUG << "Could not open" << filename;
        return;
        }

    QPixmap pixmap = grab();
    pixmap.save(&file, "PNG");
    file.close();
}



