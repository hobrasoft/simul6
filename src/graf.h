#ifndef GRAF_H
#define GRAF_H

#include "Mix.h"
#include "Engine.h"
#include <vector>
#include <QChartView>
#include <QMouseEvent>
#include <QPointF>
#include <QRectF>
#include <QValueAxis>
#include <QAction>

QT_CHARTS_USE_NAMESPACE

class Graf : public QChartView
{
    Q_OBJECT
public:
    explicit Graf(QWidget *parent = nullptr);

signals:

public slots:
    void drawGraph(const Engine *pEngine);
    void init(const Engine *pEngine);
    void setVisible(int id, bool visible);
    void setVisiblePh(bool visible);
    void setVisibleKapa(bool visible);
    void setVisibleE(bool visible);
    void setRescaleEnabled(bool rescaleEnabled) { m_rescaleEnabled = rescaleEnabled; }
    void setScale(const QRectF& rect);
    void manualScale();
    void autoscale();

private slots:
    void seriesClicked(const QPointF&);
    void subselected();
    void setAxisLabels();

private:
    QChart *m_chart;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    bool m_visiblePh;
    bool m_visibleKapa;
    bool m_visibleE;
    bool m_rescaleEnabled;

    QValueAxis *m_axis_x;
    QValueAxis *m_axis_y;
    QAction    *m_actionRescale;
    QAction    *m_actionManualScale;
    QAction    *m_actionSetAxisLabels;

    const Engine *m_engine;
};

#endif // GRAF_H
