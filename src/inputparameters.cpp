#include "inputparameters.h"
#include "ui_inputparameters.h"
#include "math.h"
#include "simul6.h"
#include "pdebug.h"
#include <QTimer>

InputParameters::InputParameters(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputParameters)
{
    ui->setupUi(this);
    enableInputs();
    connect(ui->f_optimize_dt, &QCheckBox::stateChanged, this, &InputParameters::enableInputs);
    connect(ui->f_constant_voltage, &QCheckBox::stateChanged, this, &InputParameters::voltageCheckboxChanged);
    connect(ui->f_constant_current, &QCheckBox::stateChanged, this, &InputParameters::currentCheckboxChanged);
    connect(ui->f_maxerr, QOverload<int>::of(&QSpinBox::valueChanged), [this](int) {
        emit errHChanged(getErrH());
        });
    connect(ui->f_detector, &QCheckBox::stateChanged, this, &InputParameters::detectorCheckboxChanged);
    connect(ui->f_detector_position, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &InputParameters::detectorPositionChanged);
    connect (Simul6::instance(), &Simul6::caplenChanged, this, &InputParameters::setCaplen);

    QTimer::singleShot(1, this, [this](){
        emit detectorChanged(ui->f_detector->isChecked());
        });
}

InputParameters::~InputParameters()
{
    delete ui;
}


void InputParameters::setCaplen(double x) {
    ui->f_detector_position->setMaximum(x);
}


void InputParameters::setConstantVoltage(bool x) {
    // PDEBUG << x;
    ui->f_constant_current->setChecked(!x);
    ui->f_constant_voltage->setChecked(x);
    ui->f_current->setEnabled(!x);
    ui->f_voltage->setEnabled(x);
}


void InputParameters::voltageCheckboxChanged() {
    bool checked = ui->f_constant_voltage->isChecked();
    ui->f_constant_current->setChecked(!checked);
    ui->f_current->setEnabled(!checked);
    ui->f_voltage->setEnabled(checked);
}

void InputParameters::currentCheckboxChanged() {
    bool checked = ui->f_constant_current->isChecked();
    ui->f_constant_voltage->setChecked(!checked);
    ui->f_voltage->setEnabled(!checked);
    ui->f_current->setEnabled(checked);
}

void InputParameters::detectorCheckboxChanged() {
    bool checked = ui->f_detector->isChecked();
    ui->f_detector_position->setEnabled(checked);
    ui->f_detector_label->setEnabled(checked);
    emit detectorChanged(checked);
}

bool InputParameters::getConstantVoltage() const {
    return ui->f_constant_voltage->isChecked();
}

void InputParameters::enableInputs() {
    ui->f_dt->setEnabled(!ui->f_optimize_dt->isChecked());
    ui->f_maxerr->setEnabled(ui->f_optimize_dt->isChecked());
    ui->f_maxerr_label->setEnabled(ui->f_optimize_dt->isChecked());
}

void InputParameters::showError(double time) {
    ui->f_error->setText(QString("%1").arg(time, 0, 'e', 3));
}

void InputParameters::showCurrent(double current) {
    ui->f_current->setValue(current);
}

void InputParameters::showVoltage(double voltage) {
    ui->f_voltage->setValue(voltage);
}

void InputParameters::showDt(double dt) {
    ui->f_currentDt->setText(QString("%1").arg(dt, 0, 'g', 4));
}

double InputParameters::getVoltage() const {
    return ui->f_voltage->value();
}

double InputParameters::getCurrent() const {
    return ui->f_current->value();
}

double InputParameters::getErrH() const {
    return pow(10, ui->f_maxerr->value());
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

void InputParameters::setCurrent(double x) {
    ui->f_current->setValue(x);
}


