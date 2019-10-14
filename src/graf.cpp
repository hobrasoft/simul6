#include "graf.h"
#include "omp.h"
#include "pdebug.h"
#include "constituentseries.h"
#include <iostream>
#include <cmath>
#include <QValueAxis>

Graf::Graf(QWidget *parent) : QChartView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setRubberBand(QChartView::RectangleRubberBand);
    m_chart = new QChart();
    setChart(m_chart);

    m_db = nullptr;
    // m_db = new Db::Database("abcd.simul6.sqlite3", this);
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
    double maximum = 0;
    m_chart->removeAllSeries();
    QList<QAbstractAxis *> axislistX = m_chart->axes(Qt::Horizontal);
    for (int i=0; i<axislistX.size(); i++) {
        m_chart->removeAxis(axislistX[i]);
        }
    QList<QAbstractAxis *> axislistY = m_chart->axes(Qt::Vertical);
    for (int i=0; i<axislistY.size(); i++) {
        m_chart->removeAxis(axislistY[i]);
        }
    pEngine->lock();
    size_t p = pEngine->getNp(); // points
    double inc_x = pEngine->getCapLen() / p;

    int id = 0;
    for (auto &sample : pEngine->getMix().getSamples()) {
        QLineSeries *series = new ConstituentSeries(sample, this);
        m_chart->addSeries(series);

        double x = 0;
        for (unsigned int i = 0; i <= p; i++){
            series->append(QPointF(x * 1000.0, sample.getA(0, i)));
            if (sample.getA(0, i) > maximum)  {
                maximum = sample.getA(0,i);
                }
            x += inc_x;
            }
        id += 1;
        }

    QLineSeries *series = new QLineSeries(this);
    series->setName(tr("pH"));
    series->setUseOpenGL(true);
    double x = 0;
    auto hpl = pEngine->getHpl();
    for (unsigned int i = 0; i <= p; i++){
        if (hpl[i] > 0) {
            double pH = -log(hpl[i]) / log(10);
            if (pH > maximum) { 
                maximum = pH;
                }
            series->append(QPointF(x * 1000.0, pH));
            }
        x += inc_x;
        }
    pEngine->unlock();

    m_chart->addSeries(series);
    m_chart->legend()->setVisible(false);

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
    QValueAxis *axisY = new QValueAxis(this);
    axisY->setRange(-0.09 * maximum, 1.09 * maximum);
    axisY->setTickAnchor(0);
    axisY->setTickInterval(ytickInterval);
    axisY->setTickType(QValueAxis::TicksDynamic);
    m_chart->addAxis(axisY, Qt::AlignLeft);

    double mcaplen = 1000 * pEngine->getCapLen();
    double xtickInterval \
        = (mcaplen < 2) ? 0.1
        : (mcaplen < 3) ? 0.5
        : (mcaplen < 10) ? 1.0
        : (mcaplen < 20) ? 2.0
        : (mcaplen < 50) ? 5.0
        : (mcaplen < 200) ? 10.0
        : (mcaplen < 300) ? 50.0
        : 100;
    QValueAxis *axisX = new QValueAxis(this);
    axisX->setRange(0, mcaplen);
    axisX->setTickAnchor(0);
    axisX->setTickInterval(xtickInterval);
    axisX->setTickType(QValueAxis::TicksDynamic);
    m_chart->addAxis(axisX, Qt::AlignBottom);

    QList<QAbstractSeries *> serieslist = m_chart->series();
    for (int i=0; i<serieslist.size(); i++) {
        serieslist[i]->attachAxis(axisY);
        serieslist[i]->attachAxis(axisX);
        }

}


void Graf::setVisible(int id, bool visible) {
    PDEBUG << id << visible;
    QList<QAbstractSeries *> list = m_chart->series();
    for (int i=0; i<list.size(); i++) {
        ConstituentSeries *series = qobject_cast<ConstituentSeries *>(m_chart->series()[i]);
        if (series->internalId() == id) {
            series->setVisible(visible);
            return;
            }
        }
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
    pEngine->unlock(); 

}

