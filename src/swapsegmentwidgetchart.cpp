/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "swapsegmentwidgetchart.h"
#include <QPainter>


SwapSegmentWidgetChart::SwapSegmentWidgetChart(QWidget *parent) : QWidget(parent) {
}


void SwapSegmentWidgetChart::repaint(double len1, double len2, double len3) {
    m_len1 = len1;
    m_len2 = len2;
    m_len3 = len3;

    QWidget::repaint();
}


void SwapSegmentWidgetChart::paintEvent(QPaintEvent *) {

    double x1 = width() * m_len1 / (m_len1 + m_len2 + m_len3);
    double x2 = width() * m_len2 / (m_len1 + m_len2 + m_len3) + 0.51;

    QRect rect1(0,0,width()-1,height()-1);
    QRect rect2((int)x1,0,(int)x2,height()-1);

    QPainter painter(this);
    QBrush mgbrush(QColor(127,127,127), Qt::NoBrush);
    QBrush bgbrush(QColor(255,0,0,0),   Qt::SolidPattern);
    QBrush cpbrush(QColor(220,80,80),   Qt::SolidPattern);

    painter.setBrush(mgbrush);
    painter.drawRect(rect1);
    painter.fillRect(rect2, cpbrush);

}


