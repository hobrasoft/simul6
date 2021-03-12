#ifndef _VACourse_H_
#define _VACourse_H_

#include "Mix.h"
#include "Engine.h"
#include "grafabstract.h"
#include <vector>
#include <QMouseEvent>
#include <QPointF>
#include <QRectF>
#include <QValueAxis>
#include <QAction>
#include <QPointer>

class VACourseCache;

QT_CHARTS_USE_NAMESPACE

// #define SET_AXIS_LABELS_MANUALLY

class VACourse : public GrafAbstract {
    Q_OBJECT
public:
    explicit VACourse(QWidget *parent = nullptr);
    void setVACourseCache(VACourseCache *x) { m_detectorCache = x; }

signals:

public slots:
    void drawGraph(const Engine *pEngine);
    void init(const Engine *pEngine);
    void setRescaleEnabled(bool rescaleEnabled) { m_rescaleEnabled = rescaleEnabled; }
    void setScale(const QRectF& rect);
    void manualScale();
    void autoscale(bool lock = true);
    void setAutoscale();
    void showGlobalActions(bool) Q_DECL_OVERRIDE;

    void slotRun();
    void slotFinished();

private slots:
    void seriesClicked(const QPointF&);
    void subselected();
    void setAxisLabels();
    void saveCSV();
    void saveImage();

private:
    QChart *m_chart;
    void appendData();
    static double axisTable(double);
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    QPoint m_pressedPoint;
    bool m_rescaleEnabled;

    QValueAxis *m_axis_x;
    QValueAxis *m_axis_y;
    QAction    *m_actionRescale;
    QAction    *m_actionManualScale;
    #ifdef SET_AXIS_LABELS_MANUALLY
    QAction    *m_actionSetAxisLabels;
    #endif
    QAction    *m_actionSaveCSV;
    QAction    *m_actionSaveImage;

    bool m_initialized;
    bool m_isVisible;
    bool m_manualScaled;
    bool m_drawing;
    int  m_rescaleId;

    double m_time;
    VACourseCache *m_detectorCache;

    QPointer<const Engine> m_engine;

    enum Mode {
        ConstantVoltage, ConstantCurrent
        };

    Mode m_mode;
};

#endif // GRAF_H
