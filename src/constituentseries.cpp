/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "constituentseries.h"
#include "grafstyle.h"
#include <QBrush>
#include <QPen>

ConstituentSeries::ConstituentSeries(const Sample& sample, QObject *parent) : QLineSeries(parent) {
    m_internalId = sample.getInternalId();
    setName(sample.getName());
    setUseOpenGL(true);

    // colors
    QBrush lbrush = brush();
    lbrush.setColor(sample.color());
    QPen lpen = pen();
    lpen.setColor(sample.color());
    lpen.setWidthF(PENWIDTH);
    setPen(lpen);
    setBrush(lbrush);
    setVisible(sample.visible());

}

int ConstituentSeries::internalId() const {
    return m_internalId;
}


