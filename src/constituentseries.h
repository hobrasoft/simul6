/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _ConstituentSeries_H_
#define _ConstituentSeries_H_

#include <QLineSeries>
#include "Constituent.h"

/**
 * @brief
 */
class ConstituentSeries : public QtCharts::QLineSeries {
    Q_OBJECT
  public:
    ConstituentSeries(const Constituent&, QObject *parent);

    int internalId() const;

  private:
    int m_internalId;

};

#endif

