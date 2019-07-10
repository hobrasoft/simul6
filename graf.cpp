#include "graf.h"
#include "omp.h"
#include <iostream>
#include <cmath>
#include <QLineSeries>

Graf::Graf(QWidget *parent) : QChartView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    QLineSeries *series1 = new QLineSeries(this);
    series1->setName("Kuk");
    *series1
        << QPointF(2, 4)
        << QPointF(4, 1)
        << QPointF(6, 2)
        << QPointF(8, 7)
        << QPointF(10, 20)
        << QPointF(12, 4)
        << QPointF(14, 7)
        << QPointF(16, 2)
        << QPointF(20, 9);

    QLineSeries *series2 = new QLineSeries(this);
    for (double x=2; x<25; x +=0.01){
        double y = sin(x)*3 + 10;
        *series2 << QPointF(x, y);
    }


    QChart *chart = new QChart();

    //chart->legend()->hide();
    chart->addSeries(series1);
    chart->addSeries(series2);
    chart->createDefaultAxes();
    chart->setTitle(tr("Line Chart Graph"));

    setChart(chart);

}

void Graf::drawGraph(Mix *pMix, std::vector<double> *pHpl)
{
    std::cout << "Zde" << std::endl;
//    std::cout << "Thread " << omp_get_thread_num() << "\n";

    size_t p = pMix->getNp(); // points
    QChart *chart = new QChart();
    QLineSeries *series;

    for (auto &s : pMix->getSamples()) {
        series = new QLineSeries();
        for (unsigned int i = 0; i < p; i++){
            *series << QPointF(i, s.getA(0, i));
        }
        chart->addSeries(series);
        chart->createDefaultAxes();
    }

    series = new QLineSeries();
    for (unsigned int i = 0; i < p; i++) {
        *series << QPointF(i, -log((*pHpl)[i]) / log(10));
    }
    chart->addSeries(series);
    chart->createDefaultAxes();

    this->setChart(chart);
}
