/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _SwapSegmentWidgetChart_H_
#define _SwapSegmentWidgetChart_H_

#include <QWidget>
#include <QPaintEvent>

/**
 * @brief
 */
class SwapSegmentWidgetChart : public QWidget {
    Q_OBJECT
  public:
    SwapSegmentWidgetChart(QWidget *parent = nullptr);

    void repaint(double len1, double len2, double len3);

  private:
    void paintEvent(QPaintEvent *);

    double m_len1;
    double m_len2;
    double m_len3;

};

#endif

