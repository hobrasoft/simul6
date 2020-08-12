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
#include "grafstyle.h"


/**
 * @brief
 */
class ConductivitySeries : public ConstituentSeries {
    Q_OBJECT
  public:
    ConductivitySeries(GrafAbstract *parent) : ConstituentSeries(parent) { 
        m_name = tr("Conductivity"); 
        setName(tr("κ"));
        setUseOpenGL(true);
        QBrush kappabrush = brush();
        kappabrush.setColor(KAPPA_COLOR);
        QPen kappapen = pen();
        kappapen.setColor(KAPPA_COLOR);
        kappapen.setWidthF(PENWIDTH);
        setPen(kappapen);
        setBrush(kappabrush);
        }

  public slots:
    void slotHide() Q_DECL_OVERRIDE {
        Simul6::instance()->hideKapa();
        }
    void slotScale() Q_DECL_OVERRIDE {
        m_graf->rescaleKapa();
        }
};

#endif

