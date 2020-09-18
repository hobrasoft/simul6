#ifndef INPUTPARAMETERS_H
#define INPUTPARAMETERS_H

#include <QWidget>

namespace Ui {
class InputParameters;
}

class InputParameters : public QWidget
{
    Q_OBJECT

public:
    explicit InputParameters(QWidget *parent = nullptr);
    ~InputParameters();

    double getVoltage() const;
    double getCurrent() const;
    double getDt() const;
    double getErrH() const;
    bool optimizeDt() const;
    bool getConstantVoltage() const;
    bool detectorActive() const;

    void   setDt(double);
    void   setOptimizeDt(bool);
    void   setVoltage(double);
    void   setCurrent(double);
    void   setConstantVoltage(bool);

signals:
    void   errHChanged(double);
    void   detectorChanged(bool);
    void   detectorPositionChanged(double);

public slots:
    void showError(double);
    void showCurrent(double);
    void showVoltage(double);
    void showDt(double);

    void setCaplen(double);

private slots:
    void enableInputs();

    void voltageCheckboxChanged();
    void currentCheckboxChanged();
    void detectorCheckboxChanged();

private:
    Ui::InputParameters *ui;
};

#endif // INPUTPARAMETERS_H
