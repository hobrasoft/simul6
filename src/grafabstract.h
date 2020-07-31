#ifndef __GrafAbstract_H_
#define __GrafAbstract_H_

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

class GrafAbstract : public QChartView {
    Q_OBJECT
  public:
    explicit GrafAbstract(QWidget *parent = nullptr);

/*
    virtual void drawGraph(const Engine *pEngine) = 0;
    virtual void init(const Engine *pEngine) = 0;
    virtual void setVisible(int id, bool visible) = 0;
    virtual void setVisiblePh(bool visible) = 0;
    virtual void setVisibleKapa(bool visible) = 0;
    virtual void setVisibleE(bool visible) = 0;
    virtual void setRescaleEnabled(bool rescaleEnabled) { m_rescaleEnabled = rescaleEnabled; }
    virtual void setScale(const QRectF& rect) = 0;
    virtual void manualScale() = 0;
    virtual void autoscale() = 0;

*/

    virtual void showGlobalActions(bool) = 0;
    virtual void setDetectorPosition(double) = 0;


    virtual void rescale(int internalId) = 0;
    virtual void rescaleE() {};
    virtual void rescalePh() = 0;
    virtual void rescaleKapa() = 0;

};

#endif

