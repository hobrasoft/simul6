#ifndef COMPUTECONTROL_H
#define COMPUTECONTROL_H

#include <QGroupBox>

namespace Ui {
class ComputeControl;
}

class ComputeControl : public QGroupBox
{
    Q_OBJECT

public:
    explicit ComputeControl(QWidget *parent = nullptr);
    ~ComputeControl();
    double Caplen() const;

signals:
    void init();
    void run();
    void stop();

public slots:
    void showTime(double);
    void slotFinished();

private slots:
    void slotRunClicked();
    void slotStopClicked();

private:
    Ui::ComputeControl *ui;
};

#endif // COMPUTECONTROL_H
