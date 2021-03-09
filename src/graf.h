#ifndef GRAF_H
#define GRAF_H

#include "Mix.h"
#include "Engine.h"
#include "grafabstract.h"
#include <vector>
#include <QMouseEvent>
#include <QPointF>
#include <QRectF>
#include <QValueAxis>
#include <QAction>
#include <QLineSeries>
#include <QPointer>

QT_CHARTS_USE_NAMESPACE

// #define SET_AXIS_LABELS_MANUALLY

class Graf : public GrafAbstract {
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
    void setVisibleDetector(bool visible);
    void setRescaleEnabled(bool rescaleEnabled) { m_rescaleEnabled = rescaleEnabled; }
    void setScale(const QRectF& rect);
    void manualScale();
    void autoscale();
    void showGlobalActions(bool) Q_DECL_OVERRIDE;
    void setDetectorPosition(double) Q_DECL_OVERRIDE;
    void swap();

    void slotRun();
    void slotFinished();

    void rescalePh() Q_DECL_OVERRIDE;
    void rescaleE() Q_DECL_OVERRIDE;
    void rescaleKapa() Q_DECL_OVERRIDE;
    void rescale(int internalId) Q_DECL_OVERRIDE;

private slots:
    void seriesClicked(const QPointF&);
    void subselected();
    void setAxisLabels();
    void saveImage();

private:
    QChart *m_chart;
    static double axisTable(double);
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    QPoint m_pressedPoint;
    bool m_visiblePh;
    bool m_visibleKapa;
    bool m_visibleE;
    bool m_visibleDetector;
    bool m_rescaleEnabled;

    QValueAxis *m_axis_x;
    QValueAxis *m_axis_y;
    QAction    *m_actionRescale;
    QAction    *m_actionManualScale;
    QAction    *m_actionSaveImage;
    #ifdef SET_AXIS_LABELS_MANUALLY
    QAction    *m_actionSetAxisLabels;
    #endif

    bool m_rescaleIndividually;
    bool m_rescalePh;
    bool m_rescaleE;
    bool m_rescaleKapa;
    bool m_initialized;
    int  m_rescaleId;

    QLineSeries *m_detectorSeries;
    double m_detectorPosition;

    QPointer<const Engine> m_engine;
};

#endif // GRAF_H
