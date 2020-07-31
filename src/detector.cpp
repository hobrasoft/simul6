#include "detector.h"
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
#include "grafstyle.h"
#include "manualscale.h"

#define E_COLOR     "#00ff00"
#define PH_COLOR    "#0000ff"
#define KAPPA_COLOR "#000000"

Detector::Detector(QWidget *parent) : GrafAbstract(parent)
{
    if (MSETTINGS->guiChartAntialiasing()) {
        setRenderHint(QPainter::Antialiasing);
        }
    setRubberBand(QChartView::RectangleRubberBand);
    m_chart = new QChart();
    setChart(m_chart);
    m_rescaleIndividually = false;
    m_rescalePh = false;
    m_rescaleKapa = false;
    m_visiblePh = true;
    m_visibleKapa = true;
    m_engine = nullptr;
    m_axis_x = nullptr;
    m_axis_y = nullptr;

    m_actionRescale = new QAction(tr("Auto scale"), this);
    connect(m_actionRescale, &QAction::triggered, this, &Detector::autoscale);
    addAction(m_actionRescale);
    m_actionRescale->setEnabled(false);

    m_actionManualScale = new QAction(tr("Manual scale"), this);
    connect(m_actionManualScale, &QAction::triggered, this, &Detector::manualScale);
    addAction(m_actionManualScale);
    m_actionManualScale->setEnabled(false);

    #ifdef SET_AXIS_LABELS_MANUALLY
    m_actionSetAxisLabels = new QAction(tr("Adjust axis labels"), this);
    connect(m_actionSetAxisLabels, &QAction::triggered, this, &Detector::setAxisLabels);
    addAction(m_actionSetAxisLabels);
    m_actionSetAxisLabels->setEnabled(false);
    #endif

    setContextMenuPolicy(Qt::ActionsContextMenu);
    m_rescaleEnabled = true;
    setMouseTracking(true);
}


void Detector::init(const Engine *pEngine) {
    m_chart->removeAllSeries();

    m_engine = pEngine;
    pEngine->lock();

    int detector_position = 1900;
    double time = pEngine->getTime();

    int id = 0;
    for (auto &sample : pEngine->getMix().getSamples()) {
        ConstituentSeries *series = new ConstituentSeries(sample, this);
        connect(series, &ConstituentSeries::clicked, this, &Detector::seriesClicked);
        m_chart->addSeries(series);
        series->append(QPointF(time, sample.getA(0, detector_position)));
        }

    pEngine->unlock();
    m_chart->legend()->setVisible(false);

    setAxisLabels();
}


void Detector::showGlobalActions(bool x) {
    m_actionRescale->setVisible(x);
    m_actionManualScale->setVisible(x);
}


void Detector::drawGraph(const Engine *pEngine)
{
    pEngine->lock(); 

    int detector_position = 1900;
    m_time = pEngine->getTime();

    int id = 0;
    for (auto &sample : pEngine->getMix().getSamples()) {
        QLineSeries *series = qobject_cast<QLineSeries *>(m_chart->series()[id]);
        PDEBUG << m_time << sample.getName() << sample.getA(0, detector_position);
        series->append(QPointF(m_time, sample.getA(0, detector_position)));
        series->setVisible(sample.visible());
        id += 1;
        }

    pEngine->unlock(); 
    autoscale();
}




void Detector::autoscale() {
    PDEBUG;
    if (m_engine == nullptr) { return; }
    double maximum = 0;
    double minimum = 99999;
    m_engine->lock();
    size_t p    = m_engine->getNp();
    auto hpl    = m_engine->getHpl();
    auto mix    = m_engine->getMix();
    auto kapa   = m_engine->getKapa();
    auto efield = m_engine->getE();
    m_engine->unlock();

    unsigned int xleft = 0;
    unsigned int xright = p;
    if (m_rescaleIndividually && m_axis_x != nullptr) {
        double d_xleft  = m_axis_x->min();
        double d_xright = m_axis_x->max();
        xleft = p * d_xleft / m_time;
        xright = p * d_xright / m_time;
        }

    for (auto &sample : mix.getSamples()) {
        if (!sample.visible()) { continue; }
        if (m_rescaleIndividually && sample.getId() != m_rescaleId) { continue; }
        for (unsigned int i = xleft; i <= xright; i++){
            if (sample.getA(0, i) > maximum)  {
                maximum = sample.getA(0,i);
                }
            if (sample.getA(0, i) < minimum)  {
                minimum = sample.getA(0,i);
                }
            }
        }

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

    QRectF rect;
    rect.setTop    (minimum - 0.09 * maximum);
    rect.setBottom (1.09 * maximum);
    rect.setLeft   (0);
    rect.setRight  (m_time);
    setScale(rect.normalized());

    if (m_rescaleIndividually && m_axis_x != nullptr) {
        double d_xleft  = m_axis_x->min();
        double d_xright = m_axis_x->max();
        rect.setLeft (d_xleft);
        rect.setRight(d_xright);
        }
    repaint();
}


void Detector::rescalePh() {
    m_rescaleIndividually = true;
    m_rescalePh = true;
    autoscale();
    m_rescalePh = false;
    m_rescaleIndividually = false;
}


void Detector::rescaleKapa() {
    m_rescaleIndividually = true;
    m_rescaleKapa = true;
    autoscale();
    m_rescaleKapa = false;
    m_rescaleIndividually = false;
}

void Detector::rescale(int internalId) {
    m_rescaleIndividually = true;
    m_rescaleId = internalId;
    autoscale();
    m_rescaleId = 0;
    m_rescaleIndividually = false;
}




void Detector::manualScale() {
    double caplen = 100;
    if (m_axis_y == nullptr || m_axis_y == nullptr) {
        return;
        }
    if (m_engine != nullptr) {
        m_engine->lock();
        m_engine->unlock();
        }

    QRectF rect;
    rect.setBottom(m_axis_y->min());
    rect.setTop(m_axis_y->max());
    rect.setLeft(m_axis_x->min());
    rect.setRight(m_axis_x->max());

    ManualScale *d = new ManualScale(this);
    d->setRect(rect);
    d->setCaplen(m_time);
    if (d->exec() == QDialog::Accepted) {
        setScale(d->rect().normalized());
        }
    d->deleteLater();
}


void Detector::setAxisLabels() {
    if (m_axis_y == nullptr || m_axis_x == nullptr) { 
        return;
        }
    QPointF topleft(m_axis_x->min(), m_axis_y->min());
    QPointF rightbottom(m_axis_x->max(), m_axis_y->max());
    QRectF rect(topleft, rightbottom);
    setScale(rect.normalized());
}


// prevents the unlimited scale
void Detector::mousePressEvent(QMouseEvent *event) {
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
void Detector::mouseReleaseEvent(QMouseEvent *event) { 
    if (event->button() == Qt::RightButton) { 
        event->accept();
        QChartView::mouseReleaseEvent(event);
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


void Detector::subselected() {
    if (m_axis_x == nullptr) { return; }
    if (m_axis_y == nullptr) { return; }

    QRectF  rect        = m_chart->geometry(); // m_chart->plotArea();

    QPointF topLeft     = m_chart->mapToValue(rect.topLeft());
    QPointF bottomRight = m_chart->mapToValue(rect.bottomRight());
    QRectF rext(topLeft, bottomRight);

    PDEBUG << rect << rext.normalized() << m_chart->plotArea();

    setScale(rext.normalized());

}


void Detector::setScale(const QRectF& rect) {
    PDEBUG << rect;
    if (rect.isNull()) {
        return;
        }
    if (rect.height() <= 0 || rect.width() <= 0) {
        PDEBUG << "Invalid rect dimensions" << rect << rect.height() << rect.width();
        return;
        }
    Q_ASSERT (m_engine != nullptr);
    if (m_engine == nullptr) {
        return;
        }

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


double Detector::axisTable(double maximum) {
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


void Detector::setVisible(int id, bool visible) {
    QList<QAbstractSeries *> list = m_chart->series();
    for (int i=0; i<list.size(); i++) {
        ConstituentSeries *series = qobject_cast<ConstituentSeries *>(m_chart->series()[i]);
        if (series->internalId() == id) {
            series->setVisible(visible);
            // autoscale();
            return;
            }
        }
}


void Detector::setVisiblePh(bool visible) {
    m_visiblePh = visible;
    QList<QAbstractSeries *> list = m_chart->series();
    int i = list.size()-3;
    if (i<0) { return; }
    list[i]->setVisible(visible);
    // autoscale();
}


void Detector::setVisibleKapa(bool visible) {
    m_visibleKapa = visible;
    QList<QAbstractSeries *> list = m_chart->series();
    int i = list.size()-2;
    if (i<0) { return; }
    list[i]->setVisible(visible);
    // autoscale();
}


void Detector::seriesClicked(const QPointF& point) {
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
    auto   mix      = m_engine->getMix();
    auto   kapa     = m_engine->getKapa();
    auto   efield   = m_engine->getE();
    m_engine->unlock();

    ConstituentSeries *s2 = qobject_cast<ConstituentSeries *>(s1);
    if (s2 != nullptr && s2->internalId() != 0) {
        double minimumd = 1e99;
        double minimumy = 1e99;
        m_engine->lock();
        for (auto &sample : m_engine->getMix().getSamples()) {
            double sample_y = sample.getA(0, node);
            double distance = fabs(y - sample_y);
            if (distance < minimumd) {
                minimumd = distance;
                minimumy = sample_y;
                }
            }
        m_engine->unlock();
        PDEBUG << s2->name() << s2->internalId();
/*
        DetectorDetail *d = new DetectorDetail(this, s2->name(), "mM", x, minimumy, node);
        d->move(position);
        d->show();
        connect(d, &QObject::destroyed, s2, &ConstituentSeries::setNormalWidth);
*/
        return;
        }

    int seriescount = m_chart->series().size();
    if (s1 == m_chart->series()[seriescount-3]) {
        double pH = -log(hpl[node]) / log(10);
/*
        DetectorDetail *d = new DetectorDetail(this, tr("pH"), "", x, pH, node);
        d->move(position);
        d->show();
        connect(d, &QObject::destroyed, s2, &ConstituentSeries::setNormalWidth);
*/
        return;
        }

    if (s1 == m_chart->series()[seriescount-2]) {
        double k = kapa[node] * 1000.0;
/*
        DetectorDetail *d = new DetectorDetail(this, tr("Conductivity"), "mS/m", x, k, node);
        d->move(position);
        d->show();
        connect(d, &QObject::destroyed, s2, &ConstituentSeries::setNormalWidth);
*/
        return;
        }

    if (s1 == m_chart->series()[seriescount-1]) {
        double e = efield[node];
/*
        DetectorDetail *d = new DetectorDetail(this, tr("Electric field"), "V/m", x, e, node);
        d->move(position);
        d->show();
        connect(d, &QObject::destroyed, s2, &ConstituentSeries::setNormalWidth);
*/
        return;
        }

}

