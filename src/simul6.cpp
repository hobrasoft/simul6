#include "simul6.h"
#include "preferences.h"
#include "ui_simul6.h"
#include "computecontrol.h"
#include "simulationprofile.h"
#include "constituentsdialog.h"
#include "msettings.h"
#include "messagedialog.h"
#include "json.h"
#include "importsna.h"
#include "pdebug.h"
#include "ampholines.h"
#include "saveprogress.h"
#include "about.h"
#include "localurl.h"
#include "detector.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QSize>
#include <QMessageBox>
#include <QTimer>

#include "swapsegmentwidget.h"

Simul6 *Simul6::m_instance = nullptr;


Simul6::~Simul6()
{
    writeSettings();
    delete ui;
}


Simul6::Simul6(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Simul6)
{
    m_instance = this;
    MSettings::instance(this);
    new LocalUrl(this);
    ui->setupUi(this);
    setWindowIcon(QIcon("://icons/appicon.svg"));
    createActions();
    connect(ui->f_computeControl, &ComputeControl::init, SAVEPROGRESS, &SaveProgress::init);
    connect(ui->f_computeControl, &ComputeControl::init, this, &Simul6::initEngine);
    connect(ui->f_computeControl, &ComputeControl::init, ui->f_mixcontrol, &MixControl::init);
    connect(ui->f_computeControl, &ComputeControl::init, ui->f_simulationProfile, &SimulationProfile::init);
    connect(ui->f_computeControl, &ComputeControl::run, this, &Simul6::runEngine);
    connect(ui->f_computeControl, &ComputeControl::stop, this, &Simul6::stopEngine);

    connect(ui->f_simulationProfile, &SimulationProfile::timeChanged, ui->f_computeControl, &ComputeControl::showTime);
    connect(ui->f_simulationProfile, &SimulationProfile::timeElapsed, ui->f_computeControl, &ComputeControl::showTimeElapsed);
    connect(ui->f_simulationProfile, &SimulationProfile::timeChanged, SAVEPROGRESS, &SaveProgress::slotTimeChanged);
    connect(ui->f_simulationProfile, &SimulationProfile::finished, SAVEPROGRESS, &SaveProgress::slotFinished);
    connect(ui->f_simulationProfile, &SimulationProfile::errorChanged, ui->f_parameters, &InputParameters::showError);
    connect(ui->f_simulationProfile, &SimulationProfile::dtChanged, ui->f_parameters, &InputParameters::showDt);
    connect(ui->f_simulationProfile, &SimulationProfile::curDenChanged, ui->f_parameters, &InputParameters::showCurrent);
    connect(ui->f_simulationProfile, &SimulationProfile::voltageChanged, ui->f_parameters, &InputParameters::showVoltage);
    connect(ui->f_simulationProfile, &SimulationProfile::finished, this, &Simul6::engineFinished);
    connect(ui->f_simulationProfile, &SimulationProfile::replacedConstituent, this, &Simul6::replacedConstituent);

    connect(ui->f_parameters, &InputParameters::detectorChanged, ui->f_simulationProfile, &SimulationProfile::enableDetector);
    connect(ui->f_parameters, &InputParameters::detectorPositionChanged, ui->f_simulationProfile, &SimulationProfile::setDetectorPosition);
    connect(ui->f_parameters, &InputParameters::errHChanged, [this](double x) {
        if (nullptr == ui->f_simulationProfile->engine()) { return; }
        ui->f_simulationProfile->engine()->setErrH(x);
        });


    connect(ui->f_computeControl, &ComputeControl::visibilityChangedPh, ui->f_simulationProfile, &SimulationProfile::setVisiblePh);
    connect(ui->f_computeControl, &ComputeControl::visibilityChangedKapa, ui->f_simulationProfile, &SimulationProfile::setVisibleKapa);
    connect(ui->f_computeControl, &ComputeControl::visibilityChangedE, ui->f_simulationProfile, &SimulationProfile::setVisibleE);
    connect(ui->f_computeControl, &ComputeControl::caplenChanged, this, &Simul6::caplenChanged);
    connect(ui->f_computeControl, &ComputeControl::caplenChanged, ui->f_parameters, &InputParameters::setCaplen);
    connect(ui->f_parameters,     &InputParameters::detectorChanged, ui->f_simulationProfile, &SimulationProfile::setVisibleDetector);
    connect(ui->f_mixcontrol, &MixControl::visibilityChanged, ui->f_simulationProfile, &SimulationProfile::setVisible);
    connect(ui->f_mixcontrol, &MixControl::swap, [this](const QList<SegmentedConstituent>& constituents) {
        ui->f_simulationProfile->engine()->addMix(constituents);
        SAVEPROGRESS->saveSwap();
        ui->f_simulationProfile->swap();
        });

    QTimer::singleShot(0, this, &Simul6::init);
}


Simul6 *Simul6::instance() {
    if (m_instance == nullptr) {
        new Simul6(nullptr);
    }
    return m_instance;
}


void Simul6::init() {
    readSettings();
    setDockingWindows();
    setVisible(true);
    ui->f_dock_replay->setVisible(false);
    ui->f_replay->clear();
}


Graf *Simul6::graf() const {
    return ui->f_simulationProfile->graf();
}


Detector *Simul6::detector() const {
    return ui->f_simulationProfile->detector();
}


const MixControlModelAbstract *Simul6::mixControlModel() const {
    return ui->f_mixcontrol->model();
}


const CrosssectionModel *Simul6::crosssectionModel() const {
    return ui->f_computeControl->crosssectionModel();
}


CrosssectionModel *Simul6::crosssectionModel() {
    return ui->f_computeControl->crosssectionModel();
}


int Simul6::mixSize() const {
    return ui->f_mixcontrol->mixSize();
}


void Simul6::engineFinished() {
    ui->f_computeControl->slotFinished();

}


const Engine *Simul6::engine() const {
    return ui->f_simulationProfile->engine();
}


void Simul6::runEngine() {
    ui->f_replay->clear();
    ui->f_dock_replay->setVisible(false);
    if (ui->f_simulationProfile->engine() == nullptr) { return; }
    ui->f_simulationProfile->engine()->setTimeInterval(ui->f_computeControl->getTimeInterval());
    ui->f_simulationProfile->engine()->setTimeStop(ui->f_computeControl->getTimeStop());
    ui->f_simulationProfile->engine()->setOptimizeDt(ui->f_parameters->optimizeDt());
    ui->f_simulationProfile->engine()->setConstantVoltage(ui->f_parameters->getConstantVoltage());
    ui->f_simulationProfile->engine()->setVoltage(ui->f_parameters->getVoltage());
    ui->f_simulationProfile->engine()->setCurDen(ui->f_parameters->getCurrent());
    ui->f_simulationProfile->engine()->setTimeInterval(ui->f_computeControl->getTimeInterval());
    ui->f_simulationProfile->engine()->setDt(ui->f_parameters->getDt());
    ui->f_simulationProfile->engine()->setDetectorActive(ui->f_parameters->detectorActive());
    ui->f_simulationProfile->slotRun();
}


double Simul6::getCaplen() const {
    return ui->f_computeControl->getCapLen()/1000.0;
}


void Simul6::initEngine() {
    ui->f_replay->clear();
    ui->f_dock_replay->setVisible(false);
    ui->f_simulationProfile->createEngine(ui->f_computeControl->getNp());
    ui->f_simulationProfile->engine()->setErrH(ui->f_parameters->getErrH());
    ui->f_simulationProfile->engine()->setCapLen(ui->f_computeControl->getCapLen()/1000.0);
    ui->f_simulationProfile->engine()->setBW(ui->f_computeControl->getBW());
    ui->f_simulationProfile->engine()->setTimeInterval(ui->f_computeControl->getTimeInterval());
    ui->f_simulationProfile->engine()->setTimeStop(ui->f_computeControl->getTimeStop());
    ui->f_simulationProfile->engine()->setDt(ui->f_parameters->getDt());
    ui->f_simulationProfile->engine()->setVoltage(ui->f_parameters->getVoltage());
    ui->f_simulationProfile->engine()->setCurDen(ui->f_parameters->getCurrent());
    ui->f_simulationProfile->engine()->setConstantVoltage(ui->f_parameters->getConstantVoltage());
    ui->f_simulationProfile->engine()->setOptimizeDt(ui->f_parameters->optimizeDt());
    ui->f_simulationProfile->engine()->setMix(mixControlModel()->constituents()); // Nakrmí nový engine směsí
    ui->f_simulationProfile->engine()->setCrosssection(crosssectionModel()->sections());
    ui->f_simulationProfile->engine()->setDetectorActive(ui->f_parameters->detectorActive());
    ui->f_simulationProfile->engine()->init();
    ui->f_simulationProfile->engine()->setTime(0);
}


void Simul6::stopEngine() {
    ui->f_simulationProfile->slotStop();
}


void Simul6::saveData() {
    QString dirname = MSETTINGS->dataDirName();
    QString filename = QFileDialog::getSaveFileName(this, tr("Save simulation"), dirname,
        tr("JSON format (*.json);;"
           "Sqlite3 format (*.sqlite3)")
        ).trimmed();
    if (filename.isEmpty()) { return; }
    MSETTINGS->setDataDirName(QFileInfo(filename).absoluteDir().absolutePath());
    if (!filename.endsWith(".json", Qt::CaseInsensitive) &&
        !filename.endsWith(".sqlite3", Qt::CaseInsensitive)) {
        filename = filename.replace(QRegExp("\\.+$"),"");
        filename += ".json";
        }

    bool sqliteformat = filename.endsWith(".sqlite3", Qt::CaseInsensitive);
    bool jsonformat   = filename.endsWith(".json", Qt::CaseInsensitive);

    if (jsonformat) {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            SHOWMESSAGE(tr("Could not open or create file %1").arg(filename));
            return;
            }
        file.write(JSON::json(data()));
        file.close();
        return;
        }

    if (sqliteformat) {
        QFile::remove(filename);
        Db::Database db(filename);
        db.open();
        if (!db.isOpen()) {
            SHOWMESSAGE(tr("Could not open or create file %1").arg(filename));
            return;
            }
        db.save(data());
        return;
        }
}


QVariantMap Simul6::data() const {
    QVariantMap data;
    if (mixControlModel() == nullptr) { return data; }
    data["mix"] = ui->f_mixcontrol->mixJson();
    data["swaps"] = ui->f_mixcontrol->swapsJson();
    data["crosssection"] = crosssectionModel()->jsonData();

    QVariantMap ccontrol;
    ccontrol["caplen"] = ui->f_computeControl->getCapLen()/1000.0;
    ccontrol["zone_edge"] = ui->f_computeControl->getBWmeters();
    ccontrol["number_of_points"] = ui->f_computeControl->getNp();
    ccontrol["display_interval"] = ui->f_computeControl->getTimeInterval();
    ccontrol["stop_time"] = ui->f_computeControl->getTimeStop();
    ccontrol["dt"] = ui->f_parameters->getDt();
    ccontrol["optimize_dt"] = ui->f_parameters->optimizeDt();
    ccontrol["voltage"] = ui->f_parameters->getVoltage();
    ccontrol["curden"] = ui->f_parameters->getCurrent();
    ccontrol["constant_voltage"] = ui->f_parameters->getConstantVoltage();
    data["compute_control"] = ccontrol;
    return data;
}


void Simul6::loadData() {
    QString dirname = MSETTINGS->dataDirName();
    QString filename = QFileDialog::getOpenFileName(this, tr("Load simulation"), dirname, 
        tr("Simul6 data (*.json *.sqlite3);;"
           "Simul6 data, JSON format (*.json);;"
           "Simul6 data, Sqlite3 format (*.sqlite3);;"
           "All files (*)")
        ).trimmed();

    bool sqliteformat = filename.endsWith(".sqlite3", Qt::CaseInsensitive);
    bool jsonformat   = filename.endsWith(".json", Qt::CaseInsensitive);

    setWindowTitle(tr("Simul: ") + filename);
    if (filename.isEmpty()) { return; }
    MSETTINGS->setDataDirName(QFileInfo(filename).absoluteDir().absolutePath());

    QVariant data;
    if (jsonformat) {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            SHOWMESSAGE(tr("Could not open file %1").arg(filename));
            return;
            }
        data = JSON::data(file.readAll());
        }

    Db::Database *db = nullptr;
    if (sqliteformat) {
        db = new Db::Database(filename);
        db->open();
        if (!db->isOpen()) {
            SHOWMESSAGE(tr("Could not open file %1").arg(filename));
            return;
            }
        data = db->json();
        }

    QVariantMap ccontrol = data.toMap()["compute_control"].toMap();
    ui->f_computeControl->setCaplen(1000.0 * ccontrol["caplen"].toDouble());
    ui->f_computeControl->setBWmeters( ccontrol["zone_edge"].toDouble() );
    ui->f_computeControl->setNp( ccontrol["number_of_points"].toInt() );
    ui->f_computeControl->setTimeInterval( ccontrol["display_interval"].toDouble() );
    ui->f_computeControl->setTimeStop( ccontrol["stop_time"].toDouble() );
    ui->f_parameters->setDt( ccontrol["dt"].toDouble() );
    ui->f_parameters->setOptimizeDt( ccontrol["optimize_dt"].toBool() );
    ui->f_parameters->setVoltage( ccontrol["voltage"].toDouble() );
    ui->f_parameters->setCurrent( ccontrol["curden"].toDouble() );
    ui->f_parameters->setConstantVoltage( ccontrol.contains("constant_voltage") ? ccontrol["constant_voltage"].toBool() : true );
    ui->f_mixcontrol->resizeColumns();

    ui->f_mixcontrol->setMixJson(data.toMap()["mix"].toList());
    ui->f_mixcontrol->setSwapsJson(data.toMap()["swaps"].toList());
    if (data.toMap().contains("crosssection")) {
        crosssectionModel()->setCrosssection(data.toMap()["crosssection"].toList());
      } else {
        crosssectionModel()->reset();
        }

    ui->f_computeControl->initForm();
    initEngine();
    SAVEPROGRESS->init();

    ui->f_replay->clear();
    ui->f_dock_replay->setVisible(false);

    if (jsonformat && data.toMap().contains("simulation")) {
        ui->f_dock_replay->setVisible(true);
        ui->f_replay->setEngine(ui->f_simulationProfile->engine());
        ui->f_replay->setData(data.toMap()["simulation"].toList());
        ui->f_replay->setMixData(data.toMap()["mix"].toList());
        ui->f_replay->setSwapsData(data.toMap()["swaps"].toList());
        }

    if (sqliteformat && db->containsStepData()) {
        db->close();
        ui->f_dock_replay->setVisible(true);
        ui->f_replay->setEngine(ui->f_simulationProfile->engine());
        ui->f_replay->setData(filename);
        ui->f_replay->setMixData(data.toMap()["mix"].toList());
        ui->f_replay->setSwapsData(data.toMap()["swaps"].toList());
        delete db;
        }

    ui->f_simulationProfile->init();
}


void Simul6::hideConstituent(int internalId) {
    ui->f_mixcontrol->hideConstituent(internalId);
}


void Simul6::replacedConstituent(int replacedInternalId, int replaceeInternalId) {
    Q_UNUSED(replacedInternalId);
    ui->f_mixcontrol->disableConstituent(replaceeInternalId);
}


void Simul6::hideKapa() {
    ui->f_computeControl->setKapaVisible(false);
}


void Simul6::hidePh() {
    ui->f_computeControl->setPhVisible(false);
}


void Simul6::hideE() {
    ui->f_computeControl->setEVisible(false);
}


void Simul6::createActions() {
    QMenu *menu = new QMenu(tr("Application"), this);
    ui->menuBar->addMenu(menu);
    QAction *action;

    action = new QAction(tr("Preferences"), this);
    connect(action, &QAction::triggered, [this]() {
        Preferences preferences(this);
        preferences.exec();
        setDockingWindows();
    });
    menu->addAction(action);

    action = new QAction(tr("Quit application"), this);
    menu->addAction(action);
    connect(action, &QAction::triggered, [this]() {
        closeWindow();
    });


    menu = new QMenu(tr("Data"), this);
    ui->menuBar->addMenu(menu);
    action = new QAction(tr("Load data"), this);
    menu->addAction(action);
    connect(action, &QAction::triggered, [this]() {
        loadData();
    });

    action = new QAction(tr("Save data"), this);
    menu->addAction(action);
    connect(action, &QAction::triggered, [this]() {
        saveData();
    });


    action = new QAction(tr("Ampholytes"), this);
    connect(action, &QAction::triggered, [this]() {
        Ampholines ampholines(this);
        if (QDialog::Accepted != ampholines.exec()) { return; }
        MixControlModelAbstract *model = const_cast<MixControlModelAbstract *>(mixControlModel());
        model->add(ampholines.constituents());
    });
    menu->addAction(action);


    menu = new QMenu(tr("View"), this);
    menu->addAction(ui->f_dock_computeControl->toggleViewAction());
    menu->addAction(ui->f_dock_inputParams->toggleViewAction());
    menu->addAction(ui->f_dock_saveprogress->toggleViewAction());
    menu->addAction(ui->f_dock_composition->toggleViewAction());
    menu->addAction(ui->f_dock_replay->toggleViewAction());
    menu->addAction(ui->f_mixcontrol->toggleViewSwapAction());
    ui->menuBar->addMenu(menu);


    menu = new QMenu(tr("About"), this);
    ui->menuBar->addSeparator();
    ui->menuBar->addMenu(menu);

    action = new QAction(tr("About"), this);
    connect(action, &QAction::triggered, [this]() {
        About about(this);
        about.exec();
    });
    menu->addAction(action);

}


void Simul6::closeWindow() {
    int rc= QMessageBox::question(this,
            tr("I have a question"),
            tr("Do you really want to close the application?"),
            QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);
    if (rc == QMessageBox::Yes) {
        deleteLater();
        QCoreApplication::quit();
    }
}


void Simul6::closeEvent(QCloseEvent *event) {
    closeWindow();
    event->ignore();
}


void Simul6::readSettings() {
    resize(MSETTINGS->guiMainWindowSize());

    ui->f_dock_computeControl->restoreGeometry(MSETTINGS->guiDockComputeControlGeometry());
    ui->f_dock_inputParams->restoreGeometry(MSETTINGS->guiDockInputParamsGeometry());
    ui->f_dock_saveprogress->restoreGeometry(MSETTINGS->guiDockSaveProgressGeometry());
    ui->f_dock_replay->restoreGeometry(MSETTINGS->guiDockReplayGeometry());
    ui->f_dock_composition->restoreGeometry(MSETTINGS->guiDockCompositionGeometry());

    restoreState(MSETTINGS->guiMainWindowLayout());
}


void Simul6::writeSettings() {
    MSETTINGS->setGuiMainWindowSize(size());
    MSETTINGS->setGuiMainWindowLayout(saveState());
    MSETTINGS->setGuiDockComputeControlGeometry (ui->f_dock_computeControl->saveGeometry());
    MSETTINGS->setGuiDockInputParamsGeometry    (ui->f_dock_inputParams->saveGeometry());
    MSETTINGS->setGuiDockSaveProgressGeometry   (ui->f_dock_saveprogress->saveGeometry());
    MSETTINGS->setGuiDockReplayGeometry         (ui->f_dock_replay->saveGeometry());
    MSETTINGS->setGuiDockCompositionGeometry    (ui->f_dock_composition->saveGeometry());
}


void Simul6::setDockingWindows() {
    QDockWidget::DockWidgetFeatures features = 
        ((MSETTINGS->guiCloseable()) ? QDockWidget::DockWidgetClosable  : QDockWidget::NoDockWidgetFeatures) |
        ((MSETTINGS->guiMoveable())  ? QDockWidget::DockWidgetMovable   : QDockWidget::NoDockWidgetFeatures) |
        ((MSETTINGS->guiFloatable()) ? QDockWidget::DockWidgetFloatable : QDockWidget::NoDockWidgetFeatures) ;
    //PDEBUG << features;
    ui->f_dock_computeControl->setFeatures(features);
    ui->f_dock_inputParams->setFeatures(features);
    ui->f_dock_composition->setFeatures(features);
    ui->f_dock_saveprogress->setFeatures(features);

}


