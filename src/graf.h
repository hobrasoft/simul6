#ifndef GRAF_H
#define GRAF_H

#include "Mix.h"
#include "Engine.h"
#include "db.h"
#include <vector>
#include <QChartView>
#include <QMouseEvent>
#include <QPointF>
#include <QRectF>
#include <QValueAxis>

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
    void setVisible(int id, bool visible);
    void setVisiblePh(bool visible);
    void setVisibleKapa(bool visible);

private slots:
    void seriesClicked(const QPointF&);
    void autoscale();
    void subselected(const QRectF&);

private:
    QChart *m_chart;
    Db::Database *m_db;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    bool m_visiblePh;
    bool m_visibleKapa;

    QValueAxis *m_axis_x;
    QValueAxis *m_axis_y;

    const Engine *m_engine;
};

#endif // GRAF_H
