/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _ManualScale_H_
#define _ManualScale_H_

#include <QDialog>
#include "ui_manualscale.h"

namespace Ui {
class ManualScale;
}

/**
 * @brief
 */
class ManualScale : public QDialog {
    Q_OBJECT
  public:
    ManualScale(QWidget *parent);

    void setCaplen(double caplen);
    void setRect(const QRectF& rect);
    QRectF rect() const;

  private:
    Ui::ManualScale *ui;

};

#endif

