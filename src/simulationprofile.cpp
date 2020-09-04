#include "simulationprofile.h"
#include "ui_simulationprofile.h"
#include "Engine.h"
#include "mixcontrolmodel.h"
#include "simul6.h"
#include "pdebug.h"
#include <QTabBar>


SimulationProfile::~SimulationProfile() {
    m_thread.quit();
    m_thread.wait(900);
    m_thread.terminate();
    m_thread.wait(100);
    delete ui;
}

SimulationProfile::SimulationProfile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimulationProfile)
{
    ui->setupUi(this);

    m_thread.start();
    m_engine = nullptr;

    connect(ui->f_tabwidget, &QTabWidget::currentChanged, [this](int) {
        bool detector_visible = (ui->f_tabwidget->currentWidget() == ui->page_detector);
        ui->f_detector->setIsVisible(detector_visible);
        ui->f_detector->drawGraph(m_engine);
        });

}


Graf *SimulationProfile::graf() const {
    return ui->f_graf;
}


Detector *SimulationProfile::detector() const {
    return ui->f_detector;
}


void SimulationProfile::enableDetector(bool x) {
    ui->f_tabwidget->tabBar()->setVisible(x);
    ui->f_detector->setActive(x);
    ui->f_detector->drawGraph(m_engine);
    if (!x) {
        ui->f_tabwidget->setCurrentWidget(ui->page_graf);
        }
}


void SimulationProfile::setDetectorPosition(double x) {
    ui->f_graf->setDetectorPosition(x);
    ui->f_detector->setDetectorPosition(x);
}


void SimulationProfile::init() {
    ui->f_graf->autoscale();
    ui->f_detector->autoscale();
}


void SimulationProfile::setVisible(int id, bool visible) {
    if (m_engine != nullptr) {
        m_engine->setVisible(id, visible);
        }
    ui->f_graf->setVisible(id, visible);
    ui->f_detector->setVisible(id, visible);
}


void SimulationProfile::setVisiblePh(bool visible) {
    ui->f_graf->setVisiblePh(visible);
    ui->f_detector->setVisiblePh(visible);
}


void SimulationProfile::setVisibleKapa(bool visible) {
    ui->f_graf->setVisibleKapa(visible);
    ui->f_detector->setVisibleKapa(visible);
}


void SimulationProfile::setVisibleE(bool visible) {
    ui->f_graf->setVisibleE(visible);
}


void SimulationProfile::createEngine(int np) 
{
    if (m_engine != nullptr) {
        m_engine->deleteLater();
        }
    m_engine = new Engine(np);
    m_engine->moveToThread(&m_thread);

    connect(m_engine, &Engine::mixChanged, this, &SimulationProfile::setMix);
    connect(m_engine, &Engine::drawGraph, this, &SimulationProfile::drawGraph);
    connect(m_engine, QOverload<const Engine*>::of(&Engine::timeChanged), ui->f_detector, &Detector::appendData);

    connect(m_engine, QOverload<double>::of(&Engine::timeChanged), this, &SimulationProfile::timeChanged);
    connect(m_engine, &Engine::errorChanged, this, &SimulationProfile::errorChanged);
    connect(m_engine, &Engine::curDenChanged, this, &SimulationProfile::curDenChanged);
    connect(m_engine, &Engine::voltageChanged, this, &SimulationProfile::voltageChanged);
    connect(m_engine, &Engine::dtChanged, this, &SimulationProfile::dtChanged);
    connect(m_engine, &Engine::finished, this, &SimulationProfile::finished);
    connect(m_engine, &Engine::replacedConstituent, this, &SimulationProfile::replacedConstituent);
}


void SimulationProfile::setMix(const Engine *engine) {
    ui->f_graf->init(engine);
    ui->f_detector->init(engine);
}


void SimulationProfile::drawGraph(const Engine *engine) {
    ui->f_graf->drawGraph(engine);
    ui->f_detector->drawGraph(engine);
}


void SimulationProfile::slotRun() {
    Q_ASSERT(m_engine != nullptr);
    if (m_engine == nullptr) { return; }
    m_engine->run();
}


void SimulationProfile::slotStop() {
    Q_ASSERT(m_engine != nullptr);
    if (m_engine == nullptr) { return; }
    m_engine->stop();
}

