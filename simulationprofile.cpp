#include "simulationprofile.h"
#include "ui_simulationprofile.h"
#include "Engine.h"
#include "mixcontrolmodel.h"
#include "simul6.h"
#include "pdebug.h"

SimulationProfile::SimulationProfile(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::SimulationProfile)
{
    ui->setupUi(this);

    m_thread.start();
    m_engine = nullptr;
}


void SimulationProfile::createEngine(int np) 
{
    if (m_engine != nullptr) {
        m_engine->deleteLater();
        }
    m_engine = new Engine(np);
    m_engine->moveToThread(&m_thread);

    connect(m_engine, &Engine::drawGraph, ui->f_graf, &Graf::drawGraph);
    connect(m_engine, &Engine::timeChanged, this, &SimulationProfile::timeChanged);
    connect(m_engine, &Engine::errorChanged, this, &SimulationProfile::errorChanged);
    connect(m_engine, &Engine::curDenChanged, this, &SimulationProfile::curDenChanged);
    connect(m_engine, &Engine::dtChanged, this, &SimulationProfile::dtChanged);
    connect(m_engine, &Engine::finished, this, &SimulationProfile::finished);
}


void SimulationProfile::slotRun() {
    Q_ASSERT(m_engine != nullptr);
    m_engine->run();
}


void SimulationProfile::slotStop() {
    Q_ASSERT(m_engine != nullptr);
    m_engine->stop();
}

SimulationProfile::~SimulationProfile()
{
    delete ui;
}
