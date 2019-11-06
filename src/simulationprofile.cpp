#include "simulationprofile.h"
#include "ui_simulationprofile.h"
#include "Engine.h"
#include "mixcontrolmodel.h"
#include "simul6.h"
#include "pdebug.h"


SimulationProfile::~SimulationProfile() {
    m_thread.quit();
    m_thread.wait(900);
    m_thread.terminate();
    m_thread.wait(100);
    delete ui;
}

SimulationProfile::SimulationProfile(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::SimulationProfile)
{
    ui->setupUi(this);

    m_thread.start();
    m_engine = nullptr;
}


void SimulationProfile::setVisible(int id, bool visible) {
    ui->f_graf->setVisible(id, visible);
}


void SimulationProfile::setVisiblePh(bool visible) {
    ui->f_graf->setVisiblePh(visible);
}


void SimulationProfile::setVisibleKapa(bool visible) {
    ui->f_graf->setVisibleKapa(visible);
}


void SimulationProfile::createEngine(int np) 
{
    if (m_engine != nullptr) {
        m_engine->deleteLater();
        }
    m_engine = new Engine(np);
    m_engine->moveToThread(&m_thread);

    connect(m_engine, &Engine::mixChanged, ui->f_graf, &Graf::init);
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

