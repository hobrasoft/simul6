#ifndef SIMULATIONPROFILE_H
#define SIMULATIONPROFILE_H

#include <QGroupBox>
#include <QThread>
#include "mixcontrolmodel.h"
#include "Engine.h"

namespace Ui {
class SimulationProfile;
}

class SimulationProfile : public QGroupBox
{
    Q_OBJECT

public:
    explicit SimulationProfile(QWidget *parent = nullptr);
   ~SimulationProfile();

    Engine *engine() const { return m_engine; }

    void createEngine(int np);

public slots:
    void slotRun();
    void slotStop();

signals:
    void timeChanged(double);
    void errorChanged(double);
    void curDenChanged(double);
    void dtChanged(double);
    void finished();

private:
    Ui::SimulationProfile *ui;

    Engine *m_engine;
    QThread m_thread;
};

#endif // SIMULATIONPROFILE_H
