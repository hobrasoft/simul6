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
#include "grafabstract.h"
#include "grafstyle.h"

/**
 * @brief
 */
class ElectricFieldSeries : public ConstituentSeries {
    Q_OBJECT
  public:
    ElectricFieldSeries(GrafAbstract *parent) : ConstituentSeries(parent) { 
        m_name = tr("Electric Field"); 
        setName(tr("Electric field"));
        setUseOpenGL(true);
        QBrush efieldbrush = brush();
        efieldbrush.setColor(E_COLOR);
        QPen efieldpen = pen();
        efieldpen.setColor(E_COLOR);
        efieldpen.setWidthF(PENWIDTH);
        setPen(efieldpen);
        setBrush(efieldbrush);
        }

  public slots:
    void slotHide() Q_DECL_OVERRIDE {
        Simul6::instance()->hideE();
        }
    void slotScale() Q_DECL_OVERRIDE {
        m_graf->rescaleE();
        }
};

#endif

