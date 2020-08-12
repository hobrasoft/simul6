/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "manualscale.h"


ManualScale::ManualScale(QWidget *parent) : QDialog(parent), ui(new Ui::ManualScale) {
    ui->setupUi(this);
}


void ManualScale::setCaplen(double x) {
    ui->f_right->setMaximum(x*1.5);
    ui->f_left->setMinimum(x*-0.5);
    ui->f_left->setMaximum(x);
}


void ManualScale::setRect(const QRectF& rect) {
    ui->f_bottom->setValue(rect.bottom());
    ui->f_top->setValue(rect.top());
    ui->f_left->setValue(rect.left());
    ui->f_right->setValue(rect.right());
}


QRectF ManualScale::rect() const {
    QRectF rect;
    rect.setBottom(ui->f_top->value());
    rect.setTop(ui->f_bottom->value());
    rect.setLeft(ui->f_left->value());
    rect.setRight(ui->f_right->value());
    return rect.normalized();
}
