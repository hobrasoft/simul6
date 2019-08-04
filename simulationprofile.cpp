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

    m_engine = new Engine(4, 2000);  // at present this is the input point of the number of grid-points, np
    m_thread.start();
    m_engine->moveToThread(&m_thread);

    connect(m_engine, &Engine::drawGraph, ui->f_graf, &Graf::drawGraph);
    connect(m_engine, &Engine::timeChanged, this, &SimulationProfile::timeChanged);
    connect(m_engine, &Engine::errorChanged, this, &SimulationProfile::errorChanged);
    connect(m_engine, &Engine::curDenChanged, this, &SimulationProfile::curDenChanged);
    connect(m_engine, &Engine::dtChanged, this, &SimulationProfile::dtChanged);
    connect(m_engine, &Engine::finished, this, &SimulationProfile::finished);
}

Engine *SimulationProfile::getNewEngine()
{
    delete m_engine;
    m_engine = new Engine(4, 2000);
    return m_engine;
}

void SimulationProfile::slotInit() {
    const MixControlModel *model = Simul6::instance()->mixControlModel();
    for (int row=0; row<model->rowCount(); row++) {
        Constituent constituent = model->constituent(row);
        Segments segments = model->segments(row);
        // tady by se měl krmit engine daty z modelu
    }
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
