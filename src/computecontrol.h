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

public:
    double getCapLen() const;
    double getTimeStop() const;
    double getTimeInterval() const;
    int    getBW() const;
    double getBWmeters() const;
    int    getNp() const;

    void   setCaplen(double);
    void   setBWmeters(double);
    void   setNp(int);
    void   setTimeInterval(double);
    void   setTimeStop(double);

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
    void slotInitClicked();

private:
    Ui::ComputeControl *ui;
};

#endif // COMPUTECONTROL_H