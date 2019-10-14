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
    maximum = 10*floor(1+maximum/10);
    double tickInterval \
        = (maximum <= 0.5) ? 0.01 
        : (maximum <= 5.0) ? 0.1
        : (maximum <= 50.) ? 1.0
        : (maximum <= 500.) ? 10.0  
        : 100;

    m_chart->addSeries(series);
    m_chart->legend()->setVisible(false);
    m_chart->createDefaultAxes();
    QList<QAbstractAxis *> axes = m_chart->axes(Qt::Vertical);
    if (!axes.isEmpty()) {
        QValueAxis *axis = qobject_cast<QValueAxis *>(axes[0]);
        axis->setRange(-0.09 * maximum, 1.09 * maximum);
        axis->setTickAnchor(0);
        axis->setTickInterval(tickInterval);
        axis->setTickType(QValueAxis::TicksDynamic);
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

