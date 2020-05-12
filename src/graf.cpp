#include "graf.h"
#include "omp.h"
#include "pdebug.h"
#include "constituentseries.h"
#include "grafdetail.h"
#include "msettings.h"
#include <iostream>
#include <cmath>
#include <math.h>
#include <QValueAxis>
#include "grafstyle.h"
#include "manualscale.h"

#define E_COLOR     "#00ff00"
#define PH_COLOR    "#0000ff"
#define KAPPA_COLOR "#000000"

Graf::Graf(QWidget *parent) : QChartView(parent)
{
    if (MSETTINGS->guiChartAntialiasing()) {
        setRenderHint(QPainter::Antialiasing);
        }
    setRubberBand(QChartView::RectangleRubberBand);
    m_chart = new QChart();
    setChart(m_chart);
    m_visiblePh = true;
    m_visibleKapa = true;
    m_visibleE = true;
    m_engine = nullptr;
    m_axis_x = nullptr;
    m_axis_y = nullptr;

    connect(m_chart, &QChart::plotAreaChanged, this, &Graf::subselected);
    connect(m_chart, &QChart::geometryChanged, this, &Graf::subselected);

    m_actionRescale = new QAction(tr("Auto scale"), this);
    connect(m_actionRescale, &QAction::triggered, this, &Graf::autoscale);
    addAction(m_actionRescale);

    m_actionManualScale = new QAction(tr("Manual scale"), this);
    connect(m_actionManualScale, &QAction::triggered, this, &Graf::manualScale);
    addAction(m_actionManualScale);


    setContextMenuPolicy(Qt::ActionsContextMenu);
    m_rescaleEnabled = true;
    setMouseTracking(true);
}


void Graf::mouseReleaseEvent(QMouseEvent *event) { 
    PDEBUG << event->x() << event->y();
    if (event->button() != Qt::RightButton) { 
        QChartView::mouseReleaseEvent(event);
        return; 
        }
    // m_chart->zoomReset();
    // event->accept();
}


void Graf::init(const Engine *pEngine) {
    m_chart->removeAllSeries();

    m_engine = pEngine;
    pEngine->lock();
    size_t p = pEngine->getNp(); // points
    double inc_x = pEngine->getCapLen() / p;

    int id = 0;
    for (auto &sample : pEngine->getMix().getSamples()) {
        ConstituentSeries *series = new ConstituentSeries(sample, this);
        connect(series, &ConstituentSeries::clicked, this, &Graf::seriesClicked);
        m_chart->addSeries(series);

        double x = 0;
        for (unsigned int i = 0; i <= p; i++){
            series->append(QPointF(x * 1000.0, sample.getA(0, i)));
            x += inc_x;
            }
        id += 1;
        }

    ConstituentSeries *series = new ConstituentSeries(this);
    connect(series, &ConstituentSeries::clicked, this, &Graf::seriesClicked);
    series->setName(tr("pH"));
    series->setUseOpenGL(true);
    series->setVisible(m_visiblePh);
    QBrush phbrush = series->brush();
    phbrush.setColor(PH_COLOR);
    QPen phpen = series->pen();
    phpen.setColor(PH_COLOR);
    phpen.setWidthF(PENWIDTH);
    series->setPen(phpen);
    series->setBrush(phbrush);
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

    series = new ConstituentSeries(this);
    connect(series, &ConstituentSeries::clicked, this, &Graf::seriesClicked);
    series->setName(tr("κ"));
    series->setUseOpenGL(true);
    series->setVisible(m_visibleKapa);
    QBrush kappabrush = series->brush();
    kappabrush.setColor(KAPPA_COLOR);
    QPen kappapen = series->pen();
    kappapen.setColor(KAPPA_COLOR);
    kappapen.setWidthF(PENWIDTH);
    series->setPen(kappapen);
    series->setBrush(kappabrush);
    x = 0;
    auto kapa = pEngine->getKapa();
    for (unsigned int i = 0; i <= p; i++){
        series->append(QPointF(x * 1000.0, kapa[i]*100.0));
        x += inc_x;
        }
    m_chart->addSeries(series);

    series = new ConstituentSeries(this);
    connect(series, &ConstituentSeries::clicked, this, &Graf::seriesClicked);
    series->setName(tr("Electric field"));
    series->setUseOpenGL(true);
    series->setVisible(m_visibleE);
    QBrush efieldbrush = series->brush();
    efieldbrush.setColor(E_COLOR);
    QPen efieldpen = series->pen();
    efieldpen.setColor(E_COLOR);
    efieldpen.setWidthF(PENWIDTH);
    series->setPen(efieldpen);
    series->setBrush(efieldbrush);
    x = 0;
    auto efield = pEngine->getE();
    for (unsigned int i = 0; i <= p; i++){
        series->append(QPointF(x * 1000.0, fabs(efield[i]/1000.0)));
        x += inc_x;
        }
    m_chart->addSeries(series);

    pEngine->unlock();
    m_chart->legend()->setVisible(false);

    autoscale();
}


void Graf::autoscale() {
    // PDEBUG << m_rescaleEnabled;
    // if (!m_rescaleEnabled) { return; }
    if (m_engine == nullptr) { return; }
    double maximum = 0;
    m_engine->lock();
    size_t p    = m_engine->getNp();
    auto hpl    = m_engine->getHpl();
    auto mix    = m_engine->getMix();
    auto kapa   = m_engine->getKapa();
    auto efield = m_engine->getE();
    double mcaplen = 1000 * m_engine->getCapLen();
    m_engine->unlock();

    for (auto &sample : mix.getSamples()) {
        if (!sample.visible()) { continue; }
        for (unsigned int i = 0; i <= p; i++){
            if (sample.getA(0, i) > maximum)  {
                maximum = sample.getA(0,i);
                }
            }
        }

    for (unsigned int i = 0; m_visiblePh && i <= p; i++) {
        if (hpl[i] > 0) {
            double pH = -log(hpl[i]) / log(10);
            if (pH > maximum) { 
                maximum = pH;
                }
            }
        }

    for (unsigned int i = 0; m_visibleKapa && i <= p; i++) {
        if (kapa[i] * 100 > maximum) {
            maximum = kapa[i] * 100.0;
            }
        }

    for (unsigned int i = 0; m_visibleE && i <= p; i++) {
        if (fabs(efield[i] / 1000.0) > maximum) {
            maximum = fabs(efield[i] / 1000.0);
            }
        }

    QRectF rect;
    rect.setBottom (-0.09 * maximum);
    rect.setTop    (1.09 * maximum);
    rect.setLeft   (0);
    rect.setRight  (mcaplen);
    setScale(rect);
    repaint();
}


void Graf::manualScale() {
    double caplen = 100;
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
        setScale(d->rect());
        }
    d->deleteLater();
}



void Graf::setScale(const QRectF& rect) {
    QList<QAbstractAxis *> axislistX = m_chart->axes(Qt::Horizontal);
    for (int i=0; i<axislistX.size(); i++) {
        m_chart->removeAxis(axislistX[i]);
        }
    QList<QAbstractAxis *> axislistY = m_chart->axes(Qt::Vertical);
    for (int i=0; i<axislistY.size(); i++) {
        m_chart->removeAxis(axislistY[i]);
        }

    m_axis_y = new QValueAxis(this);
    m_chart->addAxis(m_axis_y, Qt::AlignLeft);

    m_axis_x = new QValueAxis(this);
    m_chart->addAxis(m_axis_x, Qt::AlignBottom);

    double mcaplen = rect.right();
    double maximum = rect.top();
    m_axis_y->setRange(rect.bottom(), rect.top());
    m_axis_x->setRange(rect.left(), rect.right());

    #if QT_VERSION > 0x050c00
    double ytickInterval \
        = (maximum <= 0.000003) ? 0.0000001 
        : (maximum <= 0.00003) ? 0.000001 
        : (maximum <= 0.0003) ? 0.00001 
        : (maximum <= 0.003) ? 0.0001 
        : (maximum <= 0.03) ? 0.01 
        : (maximum <= 0.3) ? 0.01 
        : (maximum <= 2.0) ? 0.2
        : (maximum <= 3.0) ? 0.5
        : (maximum <= 10.) ? 1.0
        : (maximum <= 20.) ? 2.0
        : (maximum <= 50.) ? 5.0
        : (maximum <= 200.) ? 10.0  
        : (maximum <= 300.) ? 50.0  
        : 100;
    m_axis_y->setTickAnchor(0);
    m_axis_y->setTickInterval(ytickInterval);
    m_axis_y->setTickType(QValueAxis::TicksDynamic);
    #endif

    #if QT_VERSION > 0x050c00
    double xtickInterval \
        = (mcaplen < 0.00002) ? 0.000001
        : (mcaplen < 0.0002) ? 0.00001
        : (mcaplen < 0.002) ? 0.0001
        : (mcaplen < 0.02) ? 0.001
        : (mcaplen < 0.2) ? 0.01
        : (mcaplen < 2) ? 0.1
        : (mcaplen < 3) ? 0.5
        : (mcaplen < 10) ? 1.0
        : (mcaplen < 20) ? 2.0
        : (mcaplen < 50) ? 5.0
        : (mcaplen < 200) ? 10.0
        : (mcaplen < 300) ? 50.0
        : 100;
    m_axis_x->setTickAnchor(0);
    m_axis_x->setTickInterval(xtickInterval);
    m_axis_x->setTickType(QValueAxis::TicksDynamic);
    #endif

    QList<QAbstractSeries *> serieslist = m_chart->series();
    for (int i=0; i<serieslist.size(); i++) {
        serieslist[i]->attachAxis(m_axis_y);
        serieslist[i]->attachAxis(m_axis_x);
        }

}


void Graf::setVisible(int id, bool visible) {
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


void Graf::setVisiblePh(bool visible) {
    m_visiblePh = visible;
    QList<QAbstractSeries *> list = m_chart->series();
    int i = list.size()-3;
    if (i<0) { return; }
    list[i]->setVisible(visible);
    // autoscale();
}


void Graf::setVisibleKapa(bool visible) {
    m_visibleKapa = visible;
    QList<QAbstractSeries *> list = m_chart->series();
    int i = list.size()-2;
    if (i<0) { return; }
    list[i]->setVisible(visible);
    // autoscale();
}


void Graf::setVisibleE(bool visible) {
    m_visibleE = visible;
    QList<QAbstractSeries *> list = m_chart->series();
    int i = list.size()-1;
    if (i<0) { return; }
    list[i]->setVisible(visible);
    // autoscale();
}


void Graf::drawGraph(const Engine *pEngine)
{
    pEngine->lock(); 
    size_t p = pEngine->getNp(); // points
    QLineSeries *series;

    int id = 0;
    double inc_x = pEngine->getCapLen() / p;
    for (auto &sample : pEngine->getMix().getSamples()) {
        series = qobject_cast<QLineSeries *>(m_chart->series()[id]);
        double x = 0;
        QList<double> vlist;
        QVector<QPointF> plist;
        for (unsigned int i = 0; i <= p; i++){
            plist << QPointF(x * 1000.0, sample.getA(0, i));
            vlist << x;
            x += inc_x;
            }
        series->replace(plist);
        series->setVisible(sample.visible());
        id += 1;
        }

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
    auto kapa = pEngine->getKapa();
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

    pEngine->unlock(); 
}


void Graf::subselected() {
    if (m_axis_x == nullptr) { return; }
    if (m_axis_y == nullptr) { return; }

    #if QT_VERSION > 0x050c00
    QRectF rect = m_chart->geometry(); // m_chart->plotArea();
    QPointF topLeft     = m_chart->mapToValue(rect.topLeft());
    QPointF bottomRight = m_chart->mapToValue(rect.bottomRight());
    QRectF rext(topLeft, bottomRight);
    // PDEBUG << rect << rext;
    double minimum_x = rext.x();
    double minimum_y = rext.y();
    double width     = fabs(rext.width());
    double height    = fabs(rext.height());
    double xtickInterval \
        = (width <= 0.3) ? 0.01 
        : (width <= 2.0) ? 0.2
        : (width <= 3.0) ? 0.5
        : (width <= 10.) ? 1.0
        : (width <= 20.) ? 2.0
        : (width <= 50.) ? 5.0
        : (width <= 200.) ? 10.0  
        : (width <= 300.) ? 50.0  
        : 100;
    double xtickAnchor = floor(minimum_x/xtickInterval)*xtickInterval;
    m_axis_x->setTickAnchor(xtickAnchor);
    m_axis_x->setTickInterval(xtickInterval);
    m_axis_x->setTickType(QValueAxis::TicksDynamic);

    double ytickInterval \
        = (height <= 0.3) ? 0.01 
        : (height <= 2.0) ? 0.2
        : (height <= 3.0) ? 0.5
        : (height <= 10.) ? 1.0
        : (height <= 20.) ? 2.0
        : (height <= 50.) ? 5.0
        : (height <= 200.) ? 10.0  
        : (height <= 300.) ? 50.0  
        : 100;
    double ytickAnchor = floor(minimum_y/ytickInterval)*ytickInterval;
    m_axis_y->setTickAnchor(ytickAnchor);
    m_axis_y->setTickInterval(ytickInterval);
    m_axis_y->setTickType(QValueAxis::TicksDynamic);

    #endif
}


void Graf::seriesClicked(const QPointF& point) {
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
        GrafDetail *d = new GrafDetail(this, s2->name(), "mM", x, minimumy, node);
        d->move(position);
        d->show();
        connect(d, &QObject::destroyed, s2, &ConstituentSeries::setNormalWidth);
        return;
        }

    int seriescount = m_chart->series().size();
    if (s1 == m_chart->series()[seriescount-3]) {
        double pH = -log(hpl[node]) / log(10);
        GrafDetail *d = new GrafDetail(this, tr("pH"), "", x, pH, node);
        d->move(position);
        d->show();
        connect(d, &QObject::destroyed, s2, &ConstituentSeries::setNormalWidth);
        return;
        }

    if (s1 == m_chart->series()[seriescount-2]) {
        double k = kapa[node] * 1000.0;
        GrafDetail *d = new GrafDetail(this, tr("Conductivity"), "mS/m", x, k, node);
        d->move(position);
        d->show();
        connect(d, &QObject::destroyed, s2, &ConstituentSeries::setNormalWidth);
        return;
        }

    if (s1 == m_chart->series()[seriescount-1]) {
        double e = efield[node];
        GrafDetail *d = new GrafDetail(this, tr("Electric field"), "V/m", x, e, node);
        d->move(position);
        d->show();
        connect(d, &QObject::destroyed, s2, &ConstituentSeries::setNormalWidth);
        return;
        }

}

