/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _GrafDetail_H_
#define _GrafDetail_H_

#include "ui_grafdetail.h"

namespace Ui {
class Simul6;
}

/**
 * @brief
 */
class GrafDetail : public QWidget {
    Q_OBJECT
  public:
    GrafDetail(QWidget *parent, const QString& name, const QString& unit, const double x, const double y, int node);
   ~GrafDetail();
    void setXname(const QString&);
    void setXunit(const QString&);

  private:
    Ui::GrafDetail *ui;
};

#endif

