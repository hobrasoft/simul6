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

    //double current() const;
    double getVoltage() const;
    double getDt() const;
    bool optimizeDt() const;

    void   setDt(double);
    void   setOptimizeDt(bool);
    void   setVoltage(double);

public slots:
    void showError(double);
    void showcurDen(double);
    void showDt(double);

private slots:
    void enableInputs();

private:
    Ui::InputParameters *ui;
};

#endif // INPUTPARAMETERS_H