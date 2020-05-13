/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _PhSeries_H_
#define _PhSeries_H_

#include "constituentseries.h"
#include "pdebug.h"
#include "simul6.h"

/**
 * @brief
 */
class PhSeries : public ConstituentSeries {
    Q_OBJECT
  public:
    PhSeries(Graf *parent) : ConstituentSeries(parent) {}

  public slots:
    void slotHide() Q_DECL_OVERRIDE {
        Simul6::instance()->hidePh();
        }
    void slotScale() Q_DECL_OVERRIDE {
        PDEBUG;
        }
};

#endif

