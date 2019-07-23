#include "simulationprofile.h"
#include "ui_simulationprofile.h"
#include "Engine.h"

SimulationProfile::SimulationProfile(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::SimulationProfile)
{
    ui->setupUi(this);

    m_engine = new Engine(4, 800);
    m_thread.start();
    m_engine->moveToThread(&m_thread);

    connect(m_engine, &Engine::drawGraph, ui->f_graf, &Graf::drawGraph);
    connect(m_engine, &Engine::timeChanged, this, &SimulationProfile::timeChanged);
    connect(m_engine, &Engine::errorChanged, this, &SimulationProfile::errorChanged);
    connect(m_engine, &Engine::curDenChanged, this, &SimulationProfile::curDenChanged);
    connect(m_engine, &Engine::dtChanged, this, &SimulationProfile::dtChanged);
    connect(m_engine, &Engine::finished, this, &SimulationProfile::finished);
}



void SimulationProfile::slotInit() {
    m_engine->init();
}


void SimulationProfile::slotRun() {
    m_engine->run();
}


void SimulationProfile::slotStop() {
    m_engine->stop();
}

SimulationProfile::~SimulationProfile()
{
    delete ui;
}
