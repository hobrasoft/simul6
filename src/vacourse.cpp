#include "vacourse.h"
#include "vacoursecache.h"
#include "omp.h"
#include "pdebug.h"
#include "voltageseries.h"
#include "currentseries.h"
#include "grafdetail.h"
#include "msettings.h"
#include <iostream>
#include <cmath>
#include <math.h>
#include <QValueAxis>
#include <QTimer>
#include <QFileDialog>
#include "grafstyle.h"
#include "manualscale.h"
#include "printiming.h"

#define A_OFFSET 0
#define V_OFFSET 1

VACourse::VACourse(QWidget *parent) : GrafAbstract(parent)
{
    m_detectorCache = nullptr;
    if (MSETTINGS->guiChartAntialiasing()) {
        setRenderHint(QPainter::Antialiasing);
        }
    setRubberBand(QChartView::RectangleRubberBand);
    m_chart = new QChart();
    m_chart->setPlotAreaBackgroundPen(QPen(QColor("#50101010")));
    m_chart->setPlotAreaBackgroundVisible(true);
    setChart(m_chart);
    m_engine = nullptr;
    m_axis_x = nullptr;
    m_axis_y = nullptr;
    m_isVisible = true; 
    m_initialized = false;
    m_manualScaled = false;
    m_mode = ConstantVoltage;

    m_actionRescale = new QAction(tr("Auto scale"), this);
    connect(m_actionRescale, &QAction::triggered, this, &VACourse::setAutoscale);
    addAction(m_actionRescale);
    m_actionRescale->setEnabled(false);

    m_actionManualScale = new QAction(tr("Manual scale"), this);
    connect(m_actionManualScale, &QAction::triggered, this, &VACourse::manualScale);
    addAction(m_actionManualScale);
    m_actionManualScale->setEnabled(false);

    #ifdef SET_AXIS_LABELS_MANUALLY
    m_actionSetAxisLabels = new QAction(tr("Adjust axis labels"), this);
    connect(m_actionSetAxisLabels, &QAction::triggered, this, &VACourse::setAxisLabels);
    addAction(m_actionSetAxisLabels);
    m_actionSetAxisLabels->setEnabled(false);
    #endif

    m_actionSaveCSV = new QAction(tr("Save VA data to CSV file"), this);
    connect(m_actionSaveCSV, &QAction::triggered, this, &VACourse::saveCSV);
    addAction(m_actionSaveCSV);
    m_actionSaveCSV->setEnabled(false);

    m_actionSaveImage = new QAction(tr("Save VA as image"), this);
    connect(m_actionSaveImage, &QAction::triggered, this, &VACourse::saveImage);
    addAction(m_actionSaveImage);
    m_actionSaveImage->setEnabled(false);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    m_rescaleEnabled = true;
    setMouseTracking(true);

}


void VACourse::init(const Engine *pEngine) {
    PDEBUG;
    if (pEngine == nullptr) { return; }
    m_chart->removeAllSeries();
    m_initialized = false;
    m_manualScaled = false;
    m_time = 0;

    ConstituentSeries *series;
    m_engine = pEngine;
    pEngine->lock();
    m_mode = (pEngine->constantVoltage()) ? ConstantVoltage : ConstantCurrent;

    series = new CurrentSeries(this);
    connect(series, &ConstituentSeries::clicked, this, &VACourse::seriesClicked);
    auto A = pEngine->curDen;
    series->append(QPointF(m_time, A));
    series->setVisible(m_mode == ConstantVoltage);
    m_chart->addSeries(series);

    series = new VoltageSeries(this);
    connect(series, &ConstituentSeries::clicked, this, &VACourse::seriesClicked);
    auto V = pEngine->voltage;
    series->append(QPointF(m_time, V));
    series->setVisible(m_mode == ConstantCurrent);
    m_chart->addSeries(series);

    m_chart->legend()->setVisible(false);

    pEngine->unlock();

    setAxisLabels();
}


void VACourse::showGlobalActions(bool x) {
    m_actionRescale->setVisible(x);
    m_actionManualScale->setVisible(x);
    m_actionSaveCSV->setVisible(x);
    m_actionSaveImage->setVisible(x);
}


void VACourse::slotRun() {
    m_actionSaveCSV->setEnabled(false);
    m_actionSaveImage->setEnabled(false);
}


void VACourse::slotFinished() {
    m_actionSaveCSV->setEnabled(true);
    m_actionSaveImage->setEnabled(true);
}


void VACourse::appendData() {
    PDEBUG;
    if (m_detectorCache == nullptr) { return; }
    QList<QAbstractSeries*> list = m_chart->series();
    if (list.isEmpty()) { return; }

    QList<QPointF> voltage = m_detectorCache->V();
    QList<QPointF> current = m_detectorCache->A();
    m_detectorCache->clear();


    VoltageSeries *vseries = qobject_cast<VoltageSeries *>(m_chart->series()[V_OFFSET]);
    for (int i=0; i<voltage.size(); i++) {
        vseries->append(voltage[i]);
        }

    CurrentSeries *aseries = qobject_cast<CurrentSeries *>(m_chart->series()[A_OFFSET]);
    for (int i=0; i<current.size(); i++) {
        aseries->append(current[i]);
        }

    m_initialized = true;
}


void VACourse::drawGraph(const Engine *pEngine)
{
    PDEBUG;
    if (!m_isVisible) { return; }
    if (m_chart->series().isEmpty()) { return; }
    QLineSeries *series;
    pEngine->lock(); 
    m_mode = (pEngine->constantVoltage()) ? ConstantVoltage : ConstantCurrent;
    pEngine->unlock(); 

    series = qobject_cast<QLineSeries *>(m_chart->series()[V_OFFSET]);
    series->setVisible((m_mode == ConstantCurrent));
    PDEBUG << series->isVisible();

    series = qobject_cast<QLineSeries *>(m_chart->series()[A_OFFSET]);
    series->setVisible((m_mode == ConstantVoltage));
    PDEBUG << series->isVisible();

    pEngine->lock(); 
    m_time = pEngine->getTime();
    appendData();
    pEngine->unlock(); 

    autoscale();
}


void VACourse::setAutoscale() {
    m_manualScaled = false;
    autoscale();
}

void VACourse::autoscale() {
    PDEBUG;
    if (m_manualScaled) { return; }
    if (m_engine == nullptr) { return; }
    double maximum = -99999;
    double minimum =  99999;
    m_engine->lock();

    QLineSeries *series = qobject_cast<QLineSeries *>(m_chart->series()[0]);
    int count = series->count();

    unsigned int xleft = 1;
    unsigned int xright = count-1;

    m_mode = (m_engine->constantVoltage()) ? ConstantVoltage : ConstantCurrent;
    if (m_mode == ConstantVoltage) {
        series = qobject_cast<QLineSeries *>(m_chart->series()[A_OFFSET]);
      } else {
        series = qobject_cast<QLineSeries *>(m_chart->series()[V_OFFSET]);
        }
    for (unsigned int i = xleft; i <= xright; i++) {
        for (unsigned int i = xleft; i <= xright; i++){
            if (series->at(i).y() > maximum)  {
                maximum = series->at(i).y();
                }
            if (series->at(i).y() < minimum)  {
                minimum = series->at(i).y();
                }
            }
        }

    m_engine->unlock();

    QRectF rect;
    rect.setTop    (minimum - 0.09 * (maximum-minimum) );
    rect.setBottom (maximum + 0.09 * (maximum-minimum) );
    rect.setLeft   (0);
    rect.setRight  (m_time);

    PDEBUG << rect << minimum << maximum << m_time;
    if (rect.width() < 1e-15 || rect.height() < 1e-15) {
        return;
        }
    setScale(rect.normalized());

}

void VACourse::manualScale() {
    m_manualScaled = true;
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


void VACourse::setAxisLabels() {
    if (m_axis_y == nullptr || m_axis_x == nullptr) { 
        return;
        }
    QPointF topleft(m_axis_x->min(), m_axis_y->min());
    QPointF rightbottom(m_axis_x->max(), m_axis_y->max());
    QRectF rect(topleft, rightbottom);
    setScale(rect.normalized());
}


// prevents the unlimited scale
void VACourse::mousePressEvent(QMouseEvent *event) {
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
void VACourse::mouseReleaseEvent(QMouseEvent *event) { 
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


void VACourse::subselected() {
    if (m_axis_x == nullptr) { return; }
    if (m_axis_y == nullptr) { return; }
    m_manualScaled = true;

    QRectF  rect        = m_chart->geometry(); // m_chart->plotArea();

    QPointF topLeft     = m_chart->mapToValue(rect.topLeft());
    QPointF bottomRight = m_chart->mapToValue(rect.bottomRight());
    QRectF rext(topLeft, bottomRight);

    setScale(rext.normalized());

}


void VACourse::setScale(const QRectF& rect) {
    if (rect.isNull()) {
        return;
        }
    if (rect.height() <= 0 || rect.width() <= 0) {
        return;
        }
    Q_ASSERT (m_engine != nullptr);
    if (m_engine == nullptr) {
        return;
        }
    if (m_chart->series().isEmpty()) { return; }

    m_actionManualScale->setEnabled(true);
    m_actionRescale->setEnabled(true);
    #ifdef SET_AXIS_LABELS_MANUALLY
    m_actionSetAxisLabels->setEnabled(true);
    #endif

    QList<QAbstractSeries *> serieslist = m_chart->series();
    QList<QAbstractAxis *> axislistX = m_chart->axes(Qt::Horizontal);
    for (int i=0; i<axislistX.size(); i++) {
        m_chart->removeAxis(axislistX[i]);
        }
    QList<QAbstractAxis *> axislistY = m_chart->axes(Qt::Vertical);
    for (int i=0; i<axislistY.size(); i++) {
        m_chart->removeAxis(axislistY[i]);
        }
    for (int i=0; i<serieslist.size(); i++) {
        QList<QAbstractAxis *> axes = serieslist[i]->attachedAxes();
        for (int a=0; a<axes.size(); a++) {
            serieslist[i]->detachAxis(axes[a]);
            }
        }

    if (m_axis_x != nullptr) { m_axis_x->deleteLater(); }
    if (m_axis_y != nullptr) { m_axis_y->deleteLater(); }

    m_axis_y = new QValueAxis(this);
    m_axis_x = new QValueAxis(this);
    m_axis_y->setRange(rect.top(), rect.bottom());
    m_axis_x->setRange(rect.left(), rect.right());
    m_axis_x->setTitleText(tr("seconds"));

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


double VACourse::axisTable(double maximum) {
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


void VACourse::seriesClicked(const QPointF& point) {
    QLineSeries *s1 = qobject_cast<QLineSeries *>(sender());
    if (s1 == nullptr) { return; }
    if (m_chart->series().isEmpty()) { return; }

    QPoint position = mapToGlobal(QPoint(15,15));
    double x        = point.x();
    double y        = point.y();

    int node = 0;
    QVector<QPointF> data = s1->pointsVector();
    for (int i=0; i<data.size(); i++) {
        if (data[i].x() > point.x()) { 
            node = i - 1;
            break;
            } 
        }
    
    if (s1 == m_chart->series()[V_OFFSET]) {
        GrafDetail *d = new GrafDetail(this, tr("Voltage"), "V", x, y, node);
        d->setXname(tr("Time"));
        d->setXunit(tr("sec"));
        d->move(position);
        d->show();
//      connect(d, &QObject::destroyed, s1, &ConstituentSeries::setNormalWidth);
        return;
        }

    if (s1 == m_chart->series()[A_OFFSET]) {
        GrafDetail *d = new GrafDetail(this, tr("Current"), "A", x, y, node);
        d->setXname(tr("Time"));
        d->setXunit(tr("sec"));
        d->move(position);
        d->show();
//      connect(d, &QObject::destroyed, s1, &ConstituentSeries::setNormalWidth);
        return;
        }

}


void VACourse::saveCSV() {
    PDEBUG;
    QString dirname = MSETTINGS->exportDirName();
    QString filename = QFileDialog::getSaveFileName(this, tr("Save VA data"), dirname, tr("Csv format (*.csv)")).trimmed();
    if (filename.isEmpty()) { return; }
    MSETTINGS->setExportDirName(QFileInfo(filename).absoluteDir().absolutePath());

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        PDEBUG << "Could not open" << filename;
        return;
        }

    QLineSeries *firstSeries = qobject_cast<QLineSeries *>(m_chart->series()[0]);
    int count = firstSeries->count()-1;

    QStringList header;
    header << R"X("time")X";
    for (int s=0; s<m_chart->series().size(); s++) {
        QLineSeries *series = qobject_cast<QLineSeries *>(m_chart->series()[s]);
        header << QString(R"X("%1")X").arg(series->name());
        }
    file.write(header.join(" ").toUtf8());
    file.write("\n");

    for (int i=1; i<count; i++) {
        QStringList line;
        line << QString::number(firstSeries->at(i).x());
        for (int s=0; s<m_chart->series().size(); s++) {
            QLineSeries *series = qobject_cast<QLineSeries *>(m_chart->series()[s]);
            line << QString::number(series->at(i).y());
            }
        file.write(line.join(" ").toUtf8());
        file.write("\n");
        }
    file.close();

}


void VACourse::saveImage() {
    PDEBUG;
    QString dirname = MSETTINGS->exportDirName();
    QString filename = QFileDialog::getSaveFileName(this, tr("Save detector data"), dirname, tr("PNG format (*.png)")).trimmed();
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

