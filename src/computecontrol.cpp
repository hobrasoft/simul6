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
    connect(ui->f_init, &QPushButton::clicked, this, &ComputeControl::slotInitClicked);
    connect(ui->f_saveprogress, &QAbstractButton::clicked, [this]() {
        if (ui->f_saveprogress->isChecked()) { emit saveProgressChecked(); }
        });
    connect(ui->f_show_kapa, &QCheckBox::toggled, this, &ComputeControl::visibilityChangedKapa);
    connect(ui->f_show_ph, &QCheckBox::toggled, this, &ComputeControl::visibilityChangedPh);
}

ComputeControl::~ComputeControl()
{
    delete ui;
}


void ComputeControl::showTime(double time) {
    ui->f_time->setText(QString("%1").arg(time, 0, 'f', 2));
}


bool ComputeControl::showKapa() const {
    return ui->f_show_kapa->isChecked();
}


bool ComputeControl::showPh() const {
    return ui->f_show_ph->isChecked();
}


void ComputeControl::setSaveProgressChecked(bool x) {
    ui->f_saveprogress->setChecked(x);
}


void ComputeControl::slotInitClicked() {
    ui->f_init->setEnabled(true);
    ui->f_run->setEnabled(true);
    ui->f_stop->setEnabled(true);
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

double ComputeControl::getCapLen() const {
    return ui->f_caplen->value();
}

double ComputeControl::getTimeStop() const {
    return ui->f_timestop->value();
}

double ComputeControl::getTimeInterval() const {
    return ui->f_timeinterval->value();
}

int ComputeControl::getNp() const {
    return ui->f_np->value();
}

int ComputeControl::getBW() const {
    return (int)(((double)ui->f_np->value()) * ui->f_bw->value() / ui->f_caplen->value());
}

double ComputeControl::getBWmeters() const {
    return ui->f_bw->value() / 1000.0;
}

void   ComputeControl::setCaplen(double x) {
    ui->f_caplen->setValue(x);
}

void   ComputeControl::setBWmeters(double x) {
    ui->f_bw->setValue(x*1000.0);
}

void   ComputeControl::setNp(int x) {
    ui->f_np->setValue(x);
}

void   ComputeControl::setTimeInterval(double x) {
    ui->f_timeinterval->setValue(x);
}

void   ComputeControl::setTimeStop(double x) {
    ui->f_timestop->setValue(x);
}


