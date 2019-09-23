#include "inputparameters.h"
#include "ui_inputparameters.h"

InputParameters::InputParameters(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::InputParameters)
{
    ui->setupUi(this);
    enableInputs();
    connect(ui->f_optimize_dt, &QCheckBox::stateChanged, this, &InputParameters::enableInputs);
}

InputParameters::~InputParameters()
{
    delete ui;
}

void InputParameters::enableInputs() {
    ui->f_dt->setEnabled(!ui->f_optimize_dt->isChecked());
}

void InputParameters::showError(double time) {
    ui->f_error->setText(QString("%1").arg(time, 0, 'e', 3));
}

void InputParameters::showcurDen(double curDen) {
    ui->f_curDen->setText(QString("%1").arg(curDen, 0, 'g', 4));
}

void InputParameters::showDt(double dt) {
    ui->f_currentDt->setText(QString("%1").arg(dt, 0, 'g', 4));
}

double InputParameters::getVoltage() const {
    return ui->f_voltage->value();
}

double InputParameters::getDt() const {
    return ui->f_dt->value();
}

bool InputParameters::optimizeDt() const {
    return ui->f_optimize_dt->isChecked();
}

void InputParameters::setDt(double x) {
    ui->f_dt->setValue(x);
}

void InputParameters::setOptimizeDt(bool x) {
    ui->f_optimize_dt->setChecked(x);
}

void InputParameters::setVoltage(double x) {
    ui->f_voltage->setValue(x);
}


