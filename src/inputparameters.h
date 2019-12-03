#ifndef INPUTPARAMETERS_H
#define INPUTPARAMETERS_H

#include <QGroupBox>

namespace Ui {
class InputParameters;
}

class InputParameters : public QGroupBox
{
    Q_OBJECT

public:
    explicit InputParameters(QWidget *parent = nullptr);
    ~InputParameters();

    double getVoltage() const;
    double getCurrent() const;
    double getDt() const;
    bool optimizeDt() const;
    bool getConstantVoltage() const;

    void   setDt(double);
    void   setOptimizeDt(bool);
    void   setVoltage(double);
    void   setCurrent(double);
    void   setConstantVoltage(bool);

public slots:
    void showError(double);
    void showCurrent(double);
    void showVoltage(double);
    void showDt(double);

private slots:
    void enableInputs();

    void voltageCheckboxChanged();
    void currentCheckboxChanged();

private:
    Ui::InputParameters *ui;
};

#endif // INPUTPARAMETERS_H
