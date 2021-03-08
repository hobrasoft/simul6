#ifndef SIMULATIONPROFILE_H
#define SIMULATIONPROFILE_H

#include <QWidget>
#include <QThread>
#include "mixcontrolmodel.h"
#include "detectorcache.h"
#include "vacoursecache.h"
#include "Engine.h"

namespace Ui {
class SimulationProfile;
}

class Graf;
class Detector;

class SimulationProfile : public QWidget
{
    Q_OBJECT

public:
    explicit SimulationProfile(QWidget *parent = nullptr);
   ~SimulationProfile();

    Engine *engine() const { return m_engine; }

    void createEngine(int np);
    Graf *graf() const;
    Detector *detector() const;

public slots:
    void init();
    void swap();
    void slotRun();
    void slotStop();
    void setVisible(int id, bool visible);
    void setVisiblePh(bool visible);
    void setVisibleKapa(bool visible);
    void setVisibleE(bool visible);
    void setVisibleDetector(bool visible);

    void enableDetector(bool);
    void setDetectorPosition(double);

signals:
    void timeChanged(double);
    void timeElapsed(double);
    void errorChanged(double);
    void curDenChanged(double);
    void voltageChanged(double);
    void dtChanged(double);
    void finished();
    void replacedConstituent(int replacedInternalId, int replaceeInternalId);

private slots:
    void drawGraph(const Engine*);
    void setMix(const Engine*);
    void mixChanged(const Engine*);
    void slotFinished();

private:
    Ui::SimulationProfile *ui;

    Engine *m_engine;
    DetectorCache *m_detectorCache;
    VACourseCache *m_vacourseCache;
    QThread m_threadEngine;
    QThread m_thread1Cache;
    QThread m_thread2Cache;
};

#endif // SIMULATIONPROFILE_H
