#include "simulationprofile.h"
#include "ui_simulationprofile.h"
#include "Engine.h"
#include "mixcontrolmodel.h"
#include "simul6.h"
#include "pdebug.h"
#include <QTabBar>


SimulationProfile::~SimulationProfile() {
    m_threadEngine.quit();
    m_threadEngine.wait(900);
    m_threadEngine.terminate();
    m_threadEngine.wait(100);

    m_thread1Cache.quit();
    m_thread1Cache.wait(900);
    m_thread1Cache.terminate();
    m_thread1Cache.wait(100);

    m_thread2Cache.quit();
    m_thread2Cache.wait(900);
    m_thread2Cache.terminate();
    m_thread2Cache.wait(100);

    delete ui;
}

SimulationProfile::SimulationProfile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimulationProfile)
{
    ui->setupUi(this);

    m_threadEngine.start();
    m_engine = nullptr;

    m_thread1Cache.start();
    m_detectorCache = new DetectorCache();
    m_detectorCache->moveToThread(&m_thread1Cache);
    ui->f_detector->setDetectorCache(m_detectorCache);

    m_thread2Cache.start();
    m_vacourseCache = new VACourseCache();
    m_vacourseCache->moveToThread(&m_thread2Cache);
    ui->f_vacourse->setVACourseCache(m_vacourseCache);

    connect(ui->f_tabwidget, &QTabWidget::currentChanged, [this](int) {
        bool detector_visible = (ui->f_tabwidget->currentWidget() == ui->page_detector);
        PDEBUG << "detector visible" << detector_visible;
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
    ui->f_detector->setActive(x);
    ui->f_detector->drawGraph(m_engine);
    ui->f_tabwidget->tabBar()->setTabEnabled(4,x);  // 4 = index of Detector tab
    if (!x) {
        if (ui->f_tabwidget->currentWidget() != ui->page_detector) { return; }
        ui->f_tabwidget->setCurrentWidget(ui->page_graf);
        }
}


void SimulationProfile::setDetectorPosition(double x) {
    ui->f_graf->setDetectorPosition(x);
    ui->f_grafmju->setDetectorPosition(x);
    ui->f_graflconc->setDetectorPosition(x);
    ui->f_detector->setDetectorPosition(x);
    m_detectorCache->setDetectorPosition(x);
}


void SimulationProfile::setVisible(int id, bool visible) {
    if (m_engine != nullptr) {
        m_engine->setVisible(id, visible);
        }
    ui->f_graf->setVisible(id, visible);
    ui->f_grafmju->setVisible(id, visible);
    ui->f_graflconc->setVisible(id, visible);
    ui->f_detector->setVisible(id, visible);
}


void SimulationProfile::setVisiblePh(bool visible) {
    ui->f_graf->setVisiblePh(visible);
    ui->f_grafmju->setVisiblePh(visible);
    ui->f_graflconc->setVisiblePh(visible);
    ui->f_detector->setVisiblePh(visible);
}


void SimulationProfile::setVisibleDetector(bool visible) {
    ui->f_graf->setVisibleDetector(visible);
    ui->f_grafmju->setVisibleDetector(visible);
    ui->f_graflconc->setVisibleDetector(visible);
}


void SimulationProfile::setVisibleKapa(bool visible) {
    ui->f_graf->setVisibleKapa(visible);
    ui->f_grafmju->setVisibleKapa(visible);
    ui->f_graflconc->setVisibleKapa(visible);
    ui->f_detector->setVisibleKapa(visible);
}


void SimulationProfile::setVisibleE(bool visible) {
    ui->f_graf->setVisibleE(visible);
    ui->f_grafmju->setVisibleE(visible);
    ui->f_graflconc->setVisibleE(visible);
}


void SimulationProfile::createEngine(int np) 
{
    if (m_engine != nullptr) {
        m_engine->deleteLater();
        }
    m_engine = new Engine(np);
    m_engine->moveToThread(&m_threadEngine);
    m_engine->setDetectorCache(m_detectorCache);
    m_engine->setVACourseCache(m_vacourseCache);

    connect(m_engine, &Engine::mixChanged, this, &SimulationProfile::mixChanged);
    connect(m_engine, &Engine::drawGraph, this, &SimulationProfile::drawGraph);

    connect(m_engine, QOverload<double>::of(&Engine::timeChanged), this, &SimulationProfile::timeChanged);
    connect(m_engine, &Engine::errorChanged, this, &SimulationProfile::errorChanged);
    connect(m_engine, &Engine::curDenChanged, this, &SimulationProfile::curDenChanged);
    connect(m_engine, &Engine::voltageChanged, this, &SimulationProfile::voltageChanged);
    connect(m_engine, &Engine::dtChanged, this, &SimulationProfile::dtChanged);
    connect(m_engine, &Engine::finished, this, &SimulationProfile::finished);
    connect(m_engine, &Engine::finished, this, &SimulationProfile::slotFinished);
    connect(m_engine, &Engine::replacedConstituent, this, &SimulationProfile::replacedConstituent);
    connect(m_engine, QOverload<double>::of(&Engine::timeChanged), [this]() {
            emit timeElapsed(m_engine->startTime().msecsTo(m_engine->endTime())/1000.0);
            });
}


void SimulationProfile::init() {
    ui->f_graf->init(m_engine);
    ui->f_grafmju->init(m_engine);
    ui->f_graflconc->init(m_engine);
    ui->f_detector->init(m_engine);
    ui->f_vacourse->init(m_engine);
    ui->f_graf->autoscale();
    ui->f_grafmju->autoscale();
    ui->f_graflconc->autoscale();
    ui->f_detector->autoscale();
    ui->f_vacourse->autoscale();
}


void SimulationProfile::swap() {
    ui->f_detector->swap();
    ui->f_graf->swap();
    ui->f_grafmju->swap();
    ui->f_graflconc->swap();
}


void SimulationProfile::setMix(const Engine *engine) {
    ui->f_graf->init(engine);
    ui->f_grafmju->init(engine);
    ui->f_graflconc->init(engine);
    ui->f_detector->init(engine);
}


void SimulationProfile::mixChanged(const Engine *engine) {
    ui->f_graf->init(engine);
    ui->f_grafmju->init(engine);
    ui->f_graflconc->init(engine);
}


void SimulationProfile::drawGraph(const Engine *engine) {
    if (!engine->tryLock()) { return; }
    ui->f_detector->drawGraph(engine);
    ui->f_vacourse->drawGraph(engine);
    ui->f_graf->drawGraph(engine);
    ui->f_grafmju->drawGraph(engine);
    ui->f_graflconc->drawGraph(engine);
    engine->unlock();
}


void SimulationProfile::slotRun() {
    Q_ASSERT(m_engine != nullptr);
    if (m_engine == nullptr) { return; }
    m_engine->run();
    ui->f_detector->slotRun();
    ui->f_vacourse->slotRun();
    ui->f_graf->slotRun();
    ui->f_grafmju->slotRun();
    ui->f_graflconc->slotRun();
}


void SimulationProfile::slotFinished() {
    ui->f_detector->slotFinished();
    ui->f_vacourse->slotFinished();
    ui->f_graf->slotFinished();
    ui->f_grafmju->slotFinished();
    ui->f_graflconc->slotFinished();
}


void SimulationProfile::slotStop() {
    Q_ASSERT(m_engine != nullptr);
    if (m_engine == nullptr) { return; }
    m_engine->stop();
    ui->f_detector->drawGraph(m_engine);
    ui->f_detector->slotFinished();
    ui->f_vacourse->slotFinished();
    ui->f_graf->slotFinished();
    ui->f_grafmju->slotFinished();
    ui->f_graflconc->slotFinished();
}

