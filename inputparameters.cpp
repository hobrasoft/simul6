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

double InputParameters::Voltage() const {
    return ui->f_voltage->value();
}

double InputParameters::dt() const {
    return ui->f_dt->value();
}

bool InputParameters::optimizeDt() const {
    return ui->f_optimize_dt->isChecked();
}
