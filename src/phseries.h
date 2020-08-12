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
#include "grafabstract.h"
#include "grafstyle.h"

/**
 * @brief
 */
class PhSeries : public ConstituentSeries {
    Q_OBJECT
  public:
    PhSeries(GrafAbstract *parent) : ConstituentSeries(parent) { 
        m_name = tr("pH"); 
        setName(tr("pH"));
        setUseOpenGL(true);
        QBrush phbrush = brush();
        phbrush.setColor(PH_COLOR);
        QPen phpen = pen();
        phpen.setColor(PH_COLOR);
        phpen.setWidthF(PENWIDTH);
        setPen(phpen);
        setBrush(phbrush);
        }

  public slots:
    void slotHide() Q_DECL_OVERRIDE {
        Simul6::instance()->hidePh();
        }
    void slotScale() Q_DECL_OVERRIDE {
        m_graf->rescalePh();
        }
};

#endif

