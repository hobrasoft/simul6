#ifndef SIMULATIONPROFILE_H
#define SIMULATIONPROFILE_H

#include <QWidget>
#include <QThread>
#include "mixcontrolmodel.h"
#include "Engine.h"

namespace Ui {
class SimulationProfile;
}

class Graf;

class SimulationProfile : public QWidget
{
    Q_OBJECT

public:
    explicit SimulationProfile(QWidget *parent = nullptr);
   ~SimulationProfile();

    Engine *engine() const { return m_engine; }

    void createEngine(int np);
    Graf *graf() const;

public slots:
    void slotRun();
    void slotStop();
    void setVisible(int id, bool visible);
    void setVisiblePh(bool visible);
    void setVisibleKapa(bool visible);
    void setVisibleE(bool visible);

signals:
    void timeChanged(double);
    void errorChanged(double);
    void curDenChanged(double);
    void voltageChanged(double);
    void dtChanged(double);
    void finished();
    void replacedConstituent(int replacedInternalId, int replaceeInternalId);

private:
    Ui::SimulationProfile *ui;

    Engine *m_engine;
    QThread m_thread;
};

#endif // SIMULATIONPROFILE_H
