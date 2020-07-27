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

class GrafAbstract;

/**
 * @brief
 */
class ConstituentSeries : public QtCharts::QLineSeries {
    Q_OBJECT
  public:
    ConstituentSeries(const Sample&, GrafAbstract *parent);
    ConstituentSeries(GrafAbstract *parent);

    int internalId() const;
    QString name() const { return m_name; }

  public slots:
    void setNormalWidth();

  private slots:
    void slotHovered(const QPointF& point, bool state);
    void slotRemoveActions();
    virtual void slotHide();
    virtual void slotScale();
    void    createActions();

  protected:
    GrafAbstract    *m_graf;
    QString  m_name;

  private:
    int m_internalId;
    QAction *m_scale;
    QAction *m_hide;

};

#endif

