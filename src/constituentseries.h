/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _ConstituentSeries_H_
#define _ConstituentSeries_H_

#include <QLineSeries>
#include "Sample.h"

/**
 * @brief
 */
class ConstituentSeries : public QtCharts::QLineSeries {
    Q_OBJECT
  public:
    ConstituentSeries(const Sample&, QObject *parent);

    int internalId() const;

  private:
    int m_internalId;

};

#endif

