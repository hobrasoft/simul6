/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "grafdetail.h"

GrafDetail::GrafDetail(QWidget *parent, const QString& name, const QString& unit, const double x,  const double y, int node) : QWidget(parent, Qt::Popup) {
    ui = new Ui::GrafDetail;
    ui->setupUi(this);
    ui->f_name->setText(name);
    ui->f_unit->setText(unit);
    ui->f_x->setText(QString("%1").arg(x));
    ui->f_y->setText(QString("%1").arg(y));
    ui->f_node->setText(QString("%1").arg(node));
    // setStyleSheet("background: white; border-style: solid; border-width: 3px; border-color: black;");
}

GrafDetail::~GrafDetail() {
    delete ui;
}

