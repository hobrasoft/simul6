/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "constituentseries.h"
#include "grafstyle.h"
#include "pdebug.h"
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

    connect(this, &QXYSeries::hovered, this, &ConstituentSeries::slotHovered);
}


ConstituentSeries::ConstituentSeries(QObject *parent) : QLineSeries(parent) {
    m_internalId = 0;
    QPen lpen = pen();
    lpen.setWidthF(PENWIDTH);
    setPen(lpen);
    connect(this, &QXYSeries::hovered, this, &ConstituentSeries::slotHovered);
}


int ConstituentSeries::internalId() const {
    return m_internalId;
}


void ConstituentSeries::slotHovered(const QPointF& point, bool state) {
    Q_UNUSED(point);
    hide();
    QPen lpen = pen();
    lpen.setWidthF(PENWIDTH * ((state) ? 3.0 : 1.0) );
    setPen(lpen);
    show();
}


void ConstituentSeries::setNormalWidth() {
    hide();
    QPen lpen = pen();
    lpen.setWidthF(PENWIDTH);
    setPen(lpen);
    show();
}


