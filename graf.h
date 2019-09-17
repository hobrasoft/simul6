#ifndef GRAF_H
#define GRAF_H

#include "Mix.h"
#include "Engine.h"
#include <vector>
#include <QChartView>

QT_CHARTS_USE_NAMESPACE

class Graf : public QChartView
{
    Q_OBJECT
public:
    explicit Graf(QWidget *parent = nullptr);

signals:

public slots:
    //void drawGraph(std::vector<std::vector<double> > *pData);
    //void drawGraph(Mix *pMix, std::vector<double> *pHpl);
    void drawGraph(Engine *pEngine, std::vector<double> *pHpl);
private:
    QChart *m_chart;
};

#endif // GRAF_H
