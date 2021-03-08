/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _CurrentSeries_H_
#define _CurrentSeries_H_

#include "constituentseries.h"
#include "pdebug.h"
#include "simul6.h"
#include "grafabstract.h"
#include "grafstyle.h"

/**
 * @brief
 */
class CurrentSeries : public ConstituentSeries {
    Q_OBJECT
  public:
    CurrentSeries(GrafAbstract *parent) : ConstituentSeries(parent) { 
        m_name = tr("A"); 
        setName(tr("A"));
        setUseOpenGL(true);
        QBrush phbrush = brush();
        phbrush.setColor(A_COLOR);
        QPen phpen = pen();
        phpen.setColor(A_COLOR);
        phpen.setWidthF(PENWIDTH);
        setPen(phpen);
        setBrush(phbrush);
        }

  public slots:
};

#endif

