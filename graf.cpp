#include "graf.h"
#include "omp.h"
#include "pdebug.h"
#include <iostream>
#include <cmath>
#include <QLineSeries>

Graf::Graf(QWidget *parent) : QChartView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    QChart *chart = new QChart();
    chart->createDefaultAxes();
    setChart(chart);
}

void Graf::drawGraph(Engine *pEngine, std::vector<double> *pHpl)
{
    std::cout << "Zde" << std::endl;
//    std::cout << "Thread " << omp_get_thread_num() << "\n";

    size_t p = pEngine->getNp(); // points
    QChart *chart = new QChart();
    QLineSeries *series;

    double inc_x = pEngine->getCapLen() / p;
    // PDEBUG << inc_x << pEngine->getCapLen() << pEngine->getNp();
    for (auto &s : pEngine->getMix().getSamples()) {
        series = new QLineSeries();
        series->setName(s.getName());
        double x = 0;
        for (unsigned int i = 0; i <= p; i++){
            *series << QPointF(x, s.getA(0, i));
            x += inc_x;
        }

        chart->addSeries(series);
        chart->createDefaultAxes();
    }

    series = new QLineSeries();
    series->setName(tr("pH"));
    double x = 0;
    for (unsigned int i = 0; i <= p; i++) {
        *series << QPointF(x, -log((*pHpl)[i]) / log(10));
        x += inc_x;
    }
    chart->addSeries(series);
    chart->createDefaultAxes();

    this->setChart(chart);
}
