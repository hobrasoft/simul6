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

    m_db = nullptr;
    // m_db = new Db::Database("abcd.simul6.sqlite3", this);
    connect(m_chart, &QChart::plotAreaChanged, this, &Graf::subselected);
}


void Graf::mouseReleaseEvent(QMouseEvent *event) { 
    if (event->button() != Qt::RightButton) { 
        QChartView::mouseReleaseEvent(event);
        return; 
        }
    m_chart->zoomReset();
    event->accept();
}


void Graf::init(const Engine *pEngine) {
    m_chart->removeAllSeries();
    QList<QAbstractAxis *> axislistX = m_chart->axes(Qt::Horizontal);
    for (int i=0; i<axislistX.size(); i++) {
        m_chart->removeAxis(axislistX[i]);
        }
    QList<QAbstractAxis *> axislistY = m_chart->axes(Qt::Vertical);
    for (int i=0; i<axislistY.size(); i++) {
        m_chart->removeAxis(axislistY[i]);
        }
    m_engine = pEngine;
    pEngine->lock();
    size_t p = pEngine->getNp(); // points
    double inc_x = pEngine->getCapLen() / p;

    int id = 0;
    for (auto &sample : pEngine->getMix().getSamples()) {
        QLineSeries *series = new ConstituentSeries(sample, this);
        connect(series, &ConstituentSeries::clicked, this, &Graf::seriesClicked);
        m_chart->addSeries(series);

        double x = 0;
        for (unsigned int i = 0; i <= p; i++){
            series->append(QPointF(x * 1000.0, sample.getA(0, i)));
            x += inc_x;
            }
        id += 1;
        }

    QLineSeries *series = new QLineSeries(this);
    connect(series, &ConstituentSeries::clicked, this, &Graf::seriesClicked);
    series->setName(tr("pH"));
    series->setUseOpenGL(true);
    series->setVisible(m_visiblePh);
    QBrush phbrush = series->brush();
    phbrush.setColor(PH_COLOR);
    QPen phpen = series->pen();
    phpen.setColor(PH_COLOR);
    phpen.setWidthF(1.6);
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

    series = new QLineSeries(this);
    connect(series, &ConstituentSeries::clicked, this, &Graf::seriesClicked);
    series->setName(tr("κ"));
    series->setUseOpenGL(true);
    series->setVisible(m_visibleKapa);
    QBrush kappabrush = series->brush();
    kappabrush.setColor(KAPPA_COLOR);
    QPen kappapen = series->pen();
    kappapen.setColor(KAPPA_COLOR);
    kappapen.setWidthF(1.6);
    series->setPen(kappapen);
    series->setBrush(kappabrush);
    x = 0;
    auto kapa = pEngine->getKapa();
    for (unsigned int i = 0; i <= p; i++){
        series->append(QPointF(x * 1000.0, kapa[i]*100.0));
        x += inc_x;
        }
    m_chart->addSeries(series);

    series = new QLineSeries(this);
    connect(series, &ConstituentSeries::clicked, this, &Graf::seriesClicked);
    series->setName(tr("Electric field"));
    series->setUseOpenGL(true);
    series->setVisible(m_visibleE);
    QBrush efieldbrush = series->brush();
    efieldbrush.setColor(E_COLOR);
    QPen efieldpen = series->pen();
    efieldpen.setColor(E_COLOR);
    efieldpen.setWidthF(1.6);
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

    m_axis_y = new QValueAxis(this);
    m_chart->addAxis(m_axis_y, Qt::AlignLeft);

    m_axis_x = new QValueAxis(this);
    m_chart->addAxis(m_axis_x, Qt::AlignBottom);

    QList<QAbstractSeries *> serieslist = m_chart->series();
    for (int i=0; i<serieslist.size(); i++) {
        serieslist[i]->attachAxis(m_axis_y);
        serieslist[i]->attachAxis(m_axis_x);
        }

    autoscale();
}


void Graf::autoscale() {
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

    m_axis_y->setRange(-0.09 * maximum, 1.09 * maximum);
    #if QT_VERSION > 0x050c00
    double ytickInterval \
        = (maximum <= 0.3) ? 0.01 
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

    m_axis_x->setRange(0, mcaplen);
    #if QT_VERSION > 0x050c00
    double xtickInterval \
        = (mcaplen < 2) ? 0.1
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

}


void Graf::setVisible(int id, bool visible) {
    QList<QAbstractSeries *> list = m_chart->series();
    for (int i=0; i<list.size(); i++) {
        ConstituentSeries *series = qobject_cast<ConstituentSeries *>(m_chart->series()[i]);
        if (series->internalId() == id) {
            series->setVisible(visible);
            autoscale();
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
    autoscale();
}


void Graf::setVisibleKapa(bool visible) {
    m_visibleKapa = visible;
    QList<QAbstractSeries *> list = m_chart->series();
    int i = list.size()-2;
    if (i<0) { return; }
    list[i]->setVisible(visible);
    autoscale();
}


void Graf::setVisibleE(bool visible) {
    m_visibleE = visible;
    QList<QAbstractSeries *> list = m_chart->series();
    int i = list.size()-1;
    if (i<0) { return; }
    list[i]->setVisible(visible);
    autoscale();
}


void Graf::drawGraph(const Engine *pEngine)
{
    if (m_db != nullptr && !m_db->isOpen()) {
        m_db->open();
        }

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

        Dbt::Graf gdata;
        gdata.time = pEngine->t;
        gdata.id = id;
        gdata.values_array = vlist;
        if (m_db != nullptr) { m_db->save(gdata); }

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


void Graf::subselected(const QRectF& rect) {
    Q_UNUSED(rect);
    if (m_axis_x == nullptr) { return; }
    if (m_axis_y == nullptr) { return; }
    // m_axis_x->applyNiceNumbers();
    // m_axis_y->applyNiceNumbers();
}


void Graf::seriesClicked(const QPointF& point) {
    QLineSeries *s1 = qobject_cast<QLineSeries *>(sender());
    if (s1 == nullptr) { return; }
    if (m_engine == nullptr) { return; }
    double caplen = m_engine->getCapLen() * 1000.0;
    int np = m_engine->getNp();
    int node = np * point.x() / caplen;
    QPoint position = mapToGlobal(QPoint(15,15));

    ConstituentSeries *s2 = qobject_cast<ConstituentSeries *>(s1);
    if (s2 != nullptr) {
        GrafDetail *d = new GrafDetail(this, s2->name(), point, node);
        d->move(position);
        d->show();
        return;
        }

    int seriescount = m_chart->series().size();
    if (s1 == m_chart->series()[seriescount-3]) {
        GrafDetail *d = new GrafDetail(this, tr("pH"), point, node);
        d->move(position);
        d->show();
        return;
        }

    if (s1 == m_chart->series()[seriescount-2]) {
        GrafDetail *d = new GrafDetail(this, tr("Conductivity [1e-2 S/m]"), point, node);
        d->move(position);
        d->show();
        return;
        }

    if (s1 == m_chart->series()[seriescount-1]) {
        GrafDetail *d = new GrafDetail(this, tr("Electric field [kV/m]"), point, node);
        d->move(position);
        d->show();
        return;
        }

}

