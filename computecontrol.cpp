#include "computecontrol.h"
#include "ui_computecontrol.h"

ComputeControl::ComputeControl(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ComputeControl)
{
    ui->setupUi(this);
    connect(ui->f_init, &QPushButton::clicked, this, &ComputeControl::init);
    connect(ui->f_run, &QPushButton::clicked, this, &ComputeControl::run);
    connect(ui->f_stop, &QPushButton::clicked, this, &ComputeControl::stop);
    connect(ui->f_run, &QPushButton::clicked, this, &ComputeControl::slotRunClicked);
    connect(ui->f_stop, &QPushButton::clicked, this, &ComputeControl::slotStopClicked);
}

ComputeControl::~ComputeControl()
{
    delete ui;
}


void ComputeControl::showTime(double time) {
    ui->f_time->setText(QString("%1").arg(time, 0, 'f', 2));
}

void ComputeControl::slotRunClicked() {
    ui->f_init->setEnabled(false);
    ui->f_run->setEnabled(false);
    ui->f_stop->setEnabled(true);
}

void ComputeControl::slotStopClicked() {
    ui->f_init->setEnabled(true);
    ui->f_run->setEnabled(true);
    ui->f_stop->setEnabled(false);
}

void ComputeControl::slotFinished() {
    ui->f_init->setEnabled(true);
    ui->f_run->setEnabled(true);
    ui->f_stop->setEnabled(false);
}

double ComputeControl::CapLen() const {
    return ui->f_caplen->value();
}


