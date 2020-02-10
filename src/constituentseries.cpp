/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "constituentseries.h"
#include "grafstyle.h"
#include <QBrush>
#include <QPen>

ConstituentSeries::ConstituentSeries(const Constituent& constituent, QObject *parent) : QLineSeries(parent) {
    m_internalId = constituent.getInternalId();
    setName(constituent.getName());
    setUseOpenGL(true);

    // colors
    QBrush lbrush = brush();
    lbrush.setColor(constituent.color());
    QPen lpen = pen();
    lpen.setColor(constituent.color());
    lpen.setWidthF(PENWIDTH);
    setPen(lpen);
    setBrush(lbrush);
    setVisible(constituent.visible());

}

int ConstituentSeries::internalId() const {
    return m_internalId;
}


