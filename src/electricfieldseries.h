/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _ElectricFieldSeries_H_
#define _ElectricFieldSeries_H_

#include "constituentseries.h"
#include "pdebug.h"
#include "simul6.h"

/**
 * @brief
 */
class ElectricFieldSeries : public ConstituentSeries {
    Q_OBJECT
  public:
    ElectricFieldSeries(Graf *parent) : ConstituentSeries(parent) {}

  public slots:
    void slotHide() Q_DECL_OVERRIDE {
        Simul6::instance()->hideE();
        }
    void slotScale() Q_DECL_OVERRIDE {
        PDEBUG;
        }
};

#endif

