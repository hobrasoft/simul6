#include "graf.h"
#include "omp.h"
#include "pdebug.h"
#include "colorsgenerator.h"
#include <iostream>
#include <cmath>
#include <QLineSeries>
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
    double maximum = -99999;
    m_chart->removeAllSeries();
    pEngine->lock();
    size_t p = pEngine->getNp(); // points
    double inc_x = pEngine->getCapLen() / p;
    int id = 0;
    for (auto &sample : pEngine->getMix().getSamples()) {
        QLineSeries *series = new QLineSeries(this);
        series->setName(sample.getName());
        series->setUseOpenGL(true);

        // colors
        QBrush brush = series->brush();
        brush.setColor(sample.color());
        QPen pen = series->pen();
        pen.setColor(sample.color());
        pen.setWidthF(1.6);
        series->setPen(pen);
        series->setBrush(brush);
        series->setVisible(sample.visible());

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
            series->append(QPointF(x * 1000.0, -log(hpl[i]) / log(10)));
            }
        x += inc_x;
        }
    pEngine->unlock();

    m_chart->addSeries(series);
    m_chart->legend()->setVisible(false);
    m_chart->createDefaultAxes();
    QList<QAbstractAxis *> axes = m_chart->axes(Qt::Vertical);
    if (!axes.isEmpty()) {
        qobject_cast<QValueAxis *>(axes[0])->setRange(0, maximum);
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
        plist << QPointF(x * 1000.0, (hpl[i] > 0) ? -log(hpl[i]) : 0);
        x += inc_x;
        }
    series->replace(plist);
    pEngine->unlock(); 

}

