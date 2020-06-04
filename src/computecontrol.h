#ifndef COMPUTECONTROL_H
#define COMPUTECONTROL_H

#include <QWidget>

namespace Ui {
class ComputeControl;
}

class ComputeControl : public QWidget
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

    bool   showPh() const;
    bool   showKapa() const;
    bool   showE() const;

    void   setKapaVisible(bool);
    void   setPhVisible(bool);
    void   setEVisible(bool);

signals:
    void init();
    void run();
    void stop();
    void visibilityChangedPh(bool);
    void visibilityChangedKapa(bool);
    void visibilityChangedE(bool);
    void caplenChanged(double);

public slots:
    void showTime(double);
    void slotFinished();
    void initForm();

private slots:
    void slotRunClicked();
    void slotStopClicked();
    void slotInitClicked();
    void setParallelComputation();

private:
    Ui::ComputeControl *ui;
    int m_maxThreads;
};

#endif // COMPUTECONTROL_H
