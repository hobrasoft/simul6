/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _ConstituentSeries_H_
#define _ConstituentSeries_H_

#include <QLineSeries>
#include <QPointF>
#include <QAction>
#include "Sample.h"

class Graf;

/**
 * @brief
 */
class ConstituentSeries : public QtCharts::QLineSeries {
    Q_OBJECT
  public:
    ConstituentSeries(const Sample&, Graf *parent);
    ConstituentSeries(Graf *parent);

    int internalId() const;

  public slots:
    void setNormalWidth();

  private slots:
    void slotHovered(const QPointF& point, bool state);
    void slotRemoveActions();
    virtual void slotHide();
    virtual void slotScale();

  private:
    int m_internalId;
    QAction *m_scale;
    QAction *m_hide;
    Graf    *m_graf;
    void    createActions();

};

#endif

