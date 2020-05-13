/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _ConductivitySeries_H_
#define _ConductivitySeries_H_

#include "constituentseries.h"
#include "pdebug.h"
#include "simul6.h"
#include "graf.h"

/**
 * @brief
 */
class ConductivitySeries : public ConstituentSeries {
    Q_OBJECT
  public:
    ConductivitySeries(Graf *parent) : ConstituentSeries(parent) {}

  public slots:
    void slotHide() Q_DECL_OVERRIDE {
        Simul6::instance()->hideKapa();
        }
    void slotScale() Q_DECL_OVERRIDE {
        m_graf->rescaleKapa();
        }
};

#endif

