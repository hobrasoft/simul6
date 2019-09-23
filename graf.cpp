#include "graf.h"
#include "omp.h"
#include "pdebug.h"
#include <iostream>
#include <cmath>
#include <QLineSeries>

Graf::Graf(QWidget *parent) : QChartView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    m_chart = nullptr;
}

void Graf::drawGraph(Engine *pEngine, std::vector<double> *pHpl)
{
    if (m_chart != nullptr) {
        m_chart->deleteLater();
        }

    m_chart = new QChart();
    
    size_t p = pEngine->getNp(); // points
    QLineSeries *series;

    double inc_x = pEngine->getCapLen() / p;
    // PDEBUG << inc_x << pEngine->getCapLen() << pEngine->getNp();
    for (auto &s : pEngine->getMix().getSamples()) {
        series = new QLineSeries();
        series->setName(s.getName());
        double x = 0;
        for (unsigned int i = 0; i <= p; i++){
            *series << QPointF(x * 1000.0, s.getA(0, i));
            x += inc_x;
        }

        m_chart->addSeries(series);
        m_chart->createDefaultAxes();
    }

    series = new QLineSeries();
    series->setName(tr("pH"));
    double x = 0;
    for (unsigned int i = 0; i <= p; i++) {
        *series << QPointF(x * 1000.0, -log((*pHpl)[i]) / log(10));
        x += inc_x;
    }
    m_chart->addSeries(series);
    m_chart->createDefaultAxes();

    this->setChart(m_chart);
}

