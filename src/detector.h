#ifndef _Detector_H_
#define _Detector_H_

#include "Mix.h"
#include "Engine.h"
#include "grafabstract.h"
#include <vector>
#include <QMouseEvent>
#include <QPointF>
#include <QRectF>
#include <QValueAxis>
#include <QAction>

QT_CHARTS_USE_NAMESPACE

// #define SET_AXIS_LABELS_MANUALLY

class Detector : public GrafAbstract {
    Q_OBJECT
public:
    explicit Detector(QWidget *parent = nullptr);

signals:

public slots:
    void appendData(const Engine *pEngine);
    void drawGraph(const Engine *pEngine);
    void init(const Engine *pEngine);
    void setVisible(int id, bool visible);
    void setVisiblePh(bool visible);
    void setVisibleKapa(bool visible);
    void setRescaleEnabled(bool rescaleEnabled) { m_rescaleEnabled = rescaleEnabled; }
    void setScale(const QRectF& rect);
    void manualScale();
    void autoscale();
    void showGlobalActions(bool) Q_DECL_OVERRIDE;
    void setDetectorPosition(double) Q_DECL_OVERRIDE;

    void rescalePh();
    void rescaleKapa();
    void rescale(int internalId);
    void setIsVisible(bool x);
    void setActive(bool x) { m_active = x; }

private slots:
    void seriesClicked(const QPointF&);
    void subselected();
    void setAxisLabels();

private:
    QChart *m_chart;
    static double axisTable(double);
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    QPoint m_pressedPoint;
    bool m_visiblePh;
    bool m_visibleKapa;
    bool m_rescaleEnabled;

    QValueAxis *m_axis_x;
    QValueAxis *m_axis_y;
    QAction    *m_actionRescale;
    QAction    *m_actionManualScale;
    #ifdef SET_AXIS_LABELS_MANUALLY
    QAction    *m_actionSetAxisLabels;
    #endif

    bool m_rescaleIndividually;
    bool m_rescalePh;
    bool m_rescaleKapa;
    bool m_initialized;
    bool m_isVisible;
    bool m_active;
    bool m_manualScaled;
    int  m_rescaleId;

    double m_time;
    double m_detectorPosition;

    const Engine *m_engine;
};

#endif // GRAF_H
