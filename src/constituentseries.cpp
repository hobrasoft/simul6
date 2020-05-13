/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "constituentseries.h"
#include "grafstyle.h"
#include "graf.h"
#include "pdebug.h"
#include "simul6.h"
#include <QBrush>
#include <QPen>

ConstituentSeries::ConstituentSeries(const Sample& sample, Graf *parent) : QLineSeries(parent) {
    m_graf = parent;
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
    createActions();

    connect(this, &QXYSeries::hovered, this, &ConstituentSeries::slotHovered);
}


ConstituentSeries::ConstituentSeries(Graf *parent) : QLineSeries(parent) {
    m_graf = parent;
    m_internalId = 0;
    QPen lpen = pen();
    lpen.setWidthF(PENWIDTH);
    setPen(lpen);
    createActions();
    connect(this, &QXYSeries::hovered, this, &ConstituentSeries::slotHovered);
}


void ConstituentSeries::createActions() {
    m_scale = new QAction(tr("Scale"), this);
    m_hide  = new QAction(tr("Hide"), this);

    connect(m_hide, &QAction::triggered, this, &ConstituentSeries::slotRemoveActions);
    connect(m_hide, &QAction::triggered, this, &ConstituentSeries::slotHide);

    connect(m_scale, &QAction::triggered, this, &ConstituentSeries::slotRemoveActions);
    connect(m_scale, &QAction::triggered, this, &ConstituentSeries::slotScale);

}


void ConstituentSeries::slotRemoveActions() {
    m_graf->removeAction(m_scale);
    m_graf->removeAction(m_hide);
}


void ConstituentSeries::slotHide() {
    Simul6::instance()->hideConstituent(m_internalId);
}


void ConstituentSeries::slotScale() {
    // Simul6::instance()->hideConstituent(m_internalId);
}


int ConstituentSeries::internalId() const {
    return m_internalId;
}


void ConstituentSeries::slotHovered(const QPointF& point, bool state) {
    Q_UNUSED(point);
    bool visible = isVisible();
    hide();
    QPen lpen = pen();
    lpen.setWidthF(PENWIDTH * ((state) ? 3.0 : 1.0) );
    setPen(lpen);
    if (visible) { show(); }
    if (state) {
        m_graf->addAction(m_scale);
        m_graf->addAction(m_hide);
      } else {
        slotRemoveActions();
        }
}


void ConstituentSeries::setNormalWidth() {
    bool visible = isVisible();
    hide();
    QPen lpen = pen();
    lpen.setWidthF(PENWIDTH);
    setPen(lpen);
    if (visible) { show(); }
    slotRemoveActions();
}


