#ifndef GRAF_H
#define GRAF_H

#include "Mix.h"
#include "Engine.h"
#include "db.h"
#include <vector>
#include <QChartView>
#include <QMouseEvent>

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
    void drawGraph(const Engine *pEngine);
    void init(const Engine *pEngine);

private:
    QChart *m_chart;
    Db::Database *m_db;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
};

#endif // GRAF_H
