/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "grafdetail.h"

GrafDetail::GrafDetail(QWidget *parent, const QString& name, const QPointF& point, int node) : QWidget(parent, Qt::Popup) {
    ui = new Ui::GrafDetail;
    ui->setupUi(this);
    ui->f_name->setText(name);
    ui->f_x->setText(QString("%1").arg(point.x()));
    ui->f_y->setText(QString("%1").arg(point.y()));
    ui->f_node->setText(QString("%1").arg(node));
    // setStyleSheet("background: white; border-style: solid; border-width: 3px; border-color: black;");
}

