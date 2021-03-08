/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _VoltageSeries_H_
#define _VoltageSeries_H_

#include "constituentseries.h"
#include "pdebug.h"
#include "simul6.h"
#include "grafabstract.h"
#include "grafstyle.h"

/**
 * @brief
 */
class VoltageSeries : public ConstituentSeries {
    Q_OBJECT
  public:
    VoltageSeries(GrafAbstract *parent) : ConstituentSeries(parent) { 
        m_name = tr("V"); 
        setName(tr("Voltage"));
        setUseOpenGL(true);
        QBrush phbrush = brush();
        phbrush.setColor(V_COLOR);
        QPen phpen = pen();
        phpen.setColor(V_COLOR);
        phpen.setWidthF(PENWIDTH);
        setPen(phpen);
        setBrush(phbrush);
        }

  public slots:
};

#endif

