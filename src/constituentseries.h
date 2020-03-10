/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _ConstituentSeries_H_
#define _ConstituentSeries_H_

#include <QLineSeries>
#include <QPointF>
#include "Sample.h"

/**
 * @brief
 */
class ConstituentSeries : public QtCharts::QLineSeries {
    Q_OBJECT
  public:
    ConstituentSeries(const Sample&, QObject *parent);
    ConstituentSeries(QObject *parent);

    int internalId() const;

  public slots:
    void setNormalWidth();

  private slots:
    void slotHovered(const QPointF& point, bool state);

  private:
    int m_internalId;

};

#endif

