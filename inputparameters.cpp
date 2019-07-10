#include "inputparameters.h"
#include "ui_inputparameters.h"

InputParameters::InputParameters(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::InputParameters)
{
    ui->setupUi(this);
    enableInputs();
    connect(ui->f_optimise_dt, &QCheckBox::stateChanged, this, &InputParameters::enableInputs);
}

InputParameters::~InputParameters()
{
    delete ui;
}

void InputParameters::enableInputs() {
    ui->f_dt->setEnabled(!ui->f_optimise_dt->isChecked());
}

double InputParameters::current() const {
    return ui->f_current->value();
}

double InputParameters::dt() const {
    return ui->f_dt->value();
}
