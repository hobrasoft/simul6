/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "swapsegmentwidget.h"
#include "ui_swapsegmentwidget.h"
#include "simul6.h"
#include <QSpinBox>


SwapSegmentWidget::~SwapSegmentWidget() {
    delete ui;
}


SwapSegmentWidget::SwapSegmentWidget(QWidget *parent) : 
        QWidget(parent),
        ui(new Ui::SwapSegmentWidget) {
    ui->setupUi(this);

    connect (ui->f_ratio1, QOverload<int>::of(&QSpinBox::valueChanged),  this, &SwapSegmentWidget::recalculate);
    connect (ui->f_ratio2, QOverload<int>::of(&QSpinBox::valueChanged),  this, &SwapSegmentWidget::recalculate);
    connect (ui->f_ratio3, QOverload<int>::of(&QSpinBox::valueChanged),  this, &SwapSegmentWidget::recalculate);
    connect (Simul6::instance(), &Simul6::caplenChanged, this, &SwapSegmentWidget::recalculate);
    recalculate();
}


void SwapSegmentWidget::recalculate() {
    double caplen   = 1e3 * Simul6::instance()->getCaplen();
    double ratio1   = ui->f_ratio1->value();
    double ratio2   = ui->f_ratio2->value();
    double ratio3   = ui->f_ratio3->value();
    double ratiosum = ratio1 + ratio2 + ratio3;

    double len1 = caplen * ratio1 / ratiosum;
    double len2 = caplen * ratio2 / ratiosum;
    double len3 = caplen * ratio3 / ratiosum;

    ui->f_len1->setText(QString("%1").arg(len1,0,'g',2,QChar('0')));
    ui->f_len2->setText(QString("%1").arg(len2,0,'g',2,QChar('0')));
    ui->f_len3->setText(QString("%1").arg(len3,0,'g',2,QChar('0')));

    ui->f_chart->repaint(len1, len2, len3);
}



