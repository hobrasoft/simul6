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
#include "db.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QSize>
#include <QMessageBox>
#include <QTimer>

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
    qDebug() << "Simul6";
    ui->setupUi(this);
    createActions();
    connect(ui->f_computeControl, &ComputeControl::init, this, &Simul6::initEngine);
    connect(ui->f_computeControl, &ComputeControl::run, this, &Simul6::runEngine);
    connect(ui->f_computeControl, &ComputeControl::stop, this, &Simul6::stopEngine);

    connect(ui->f_simulationProfile, &SimulationProfile::timeChanged, ui->f_computeControl, &ComputeControl::showTime);
    connect(ui->f_simulationProfile, &SimulationProfile::errorChanged, ui->f_parameters, &InputParameters::showError);
    connect(ui->f_simulationProfile, &SimulationProfile::dtChanged, ui->f_parameters, &InputParameters::showDt);
    connect(ui->f_simulationProfile, &SimulationProfile::curDenChanged, ui->f_parameters, &InputParameters::showcurDen);
    connect(ui->f_simulationProfile, &SimulationProfile::finished, this, &Simul6::engineFinished);
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
    // ImportSna import(this);
    // import.readData("/home/tycho/Desktop/TestSNA.sna");
}


const MixControlModel *Simul6::mixControlModel() const {
    return ui->f_mixcontrol->model();
}


void Simul6::engineFinished() {
    ui->f_parameters->setEnabled(true);
    ui->f_computeControl->slotFinished();

}

void Simul6::runEngine() {
    ui->f_parameters->setEnabled(false);
    ui->f_simulationProfile->engine()->setTimeInterval(ui->f_computeControl->getTimeInterval());
    ui->f_simulationProfile->engine()->setTimeStop(ui->f_computeControl->getTimeStop());
    ui->f_simulationProfile->slotRun();
}


double Simul6::getCaplen() const {
    return ui->f_computeControl->getCapLen()/1000.0;
}


void Simul6::initEngine() {
    ui->f_simulationProfile->createEngine(ui->f_computeControl->getNp());
    ui->f_simulationProfile->engine()->setTime(0);
    ui->f_simulationProfile->engine()->setErrH(1e-7); //this must be set carefully to ensure stability of computation with optimization of dt
    ui->f_simulationProfile->engine()->setCapLen(ui->f_computeControl->getCapLen()/1000.0);
    ui->f_simulationProfile->engine()->setBW(ui->f_computeControl->getBW());
    ui->f_simulationProfile->engine()->setTimeInterval(ui->f_computeControl->getTimeInterval());
    ui->f_simulationProfile->engine()->setTimeStop(ui->f_computeControl->getTimeStop());
    ui->f_simulationProfile->engine()->setDt(ui->f_parameters->getDt());
    ui->f_simulationProfile->engine()->setVoltage(ui->f_parameters->getVoltage());
    ui->f_simulationProfile->engine()->setOptimizeDt(ui->f_parameters->optimizeDt());
    ui->f_simulationProfile->engine()->setMix(mixControlModel()->constituents(), mixControlModel()->segments()); // Nakrmí nový engine směsí
}


void Simul6::stopEngine() {
    ui->f_parameters->setEnabled(true);
    ui->f_simulationProfile->slotStop();
}


void Simul6::saveData() {
    QString dirname = MSETTINGS->dataDirName();
    QString filename = QFileDialog::getSaveFileName(this, tr("Save simulation"), dirname,
        tr("Simul6 data [.simul6.json] (*.simul6.json)")
        );
    if (filename.isEmpty()) { return; }
    filename = filename.trimmed();
    MSETTINGS->setDataDirName(QFileInfo(filename).absoluteDir().absolutePath());
    if (!filename.endsWith(".simul6.json", Qt::CaseInsensitive)) {
        filename = filename.replace(QRegExp("\\.+$"),"");
        filename += ".simul6.json";
        }
    PDEBUG << filename;
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        SHOWMESSAGE(tr("Could not open or create file %1").arg(filename));
        return;
        }
    QVariantMap data;
    data["mix"] = mixControlModel()->json();

    QVariantMap ccontrol;
    ccontrol["caplen"] = ui->f_computeControl->getCapLen()/1000.0;
    ccontrol["zone_edge"] = ui->f_computeControl->getBWmeters();
    ccontrol["number_of_points"] = ui->f_computeControl->getNp();
    ccontrol["display_interval"] = ui->f_computeControl->getTimeInterval();
    ccontrol["stop_time"] = ui->f_computeControl->getTimeStop();
    ccontrol["dt"] = ui->f_parameters->getDt();
    ccontrol["optimize_dt"] = ui->f_parameters->optimizeDt();
    ccontrol["voltage"] = ui->f_parameters->getVoltage();
    data["compute_control"] = ccontrol;

    file.write(JSON::json(data));
    file.close();
}


void Simul6::loadData() {
    QString dirname = MSETTINGS->dataDirName();
    QString filename = QFileDialog::getOpenFileName(this, tr("Load simulation"), dirname, 
        tr("Simul6 data (*.simul6.json)")
        );
    if (filename.isEmpty()) { return; }
    MSETTINGS->setDataDirName(QFileInfo(filename).absoluteDir().absolutePath());
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        SHOWMESSAGE(tr("Could not open file %1").arg(filename));
        return;
        }
    QVariant data = JSON::data(file.readAll());
    MixControlModel *model = const_cast<MixControlModel *>(mixControlModel());
    model->setJson(data.toMap()["mix"].toList());

    QVariantMap ccontrol = data.toMap()["compute_control"].toMap();
    ui->f_computeControl->setCaplen(1000.0 * ccontrol["caplen"].toDouble());
    ui->f_computeControl->setBWmeters( ccontrol["zone_edge"].toDouble() );
    ui->f_computeControl->setNp( ccontrol["number_of_points"].toInt() );
    ui->f_computeControl->setTimeInterval( ccontrol["display_interval"].toDouble() );
    ui->f_computeControl->setTimeStop( ccontrol["stop_time"].toDouble() );
    ui->f_parameters->setDt( ccontrol["dt"].toDouble() );
    ui->f_parameters->setOptimizeDt( ccontrol["optimize_dt"].toBool() );
    ui->f_parameters->setVoltage( ccontrol["voltage"].toDouble() );
    ui->f_mixcontrol->resizeColumns();

    initEngine();
}


void Simul6::createActions() {
    QMenu *menu = new QMenu(tr("Application"), this);
    ui->menuBar->addMenu(menu);
    QAction *action;

    action = new QAction(tr("Preferences"), this);
    connect(action, &QAction::triggered, [this]() {
        Preferences preferences(this);
        preferences.exec();
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

    action = new QAction(tr("Ampholines"), this);
    connect(action, &QAction::triggered, [this]() {
        Ampholines ampholines(this);
        if (QDialog::Accepted != ampholines.exec()) { return; }
        MixControlModel *model = const_cast<MixControlModel *>(mixControlModel());
        model->add(ampholines.constituents(), ampholines.segments());
    });
    menu->addAction(action);


    menu = new QMenu(tr("About"), this);
    ui->menuBar->addSeparator();
    ui->menuBar->addMenu(menu);

    action = new QAction(tr("About"), this);
    connect(action, &QAction::triggered, [this]() {
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

    restoreState(MSETTINGS->guiMainWindowLayout1(), 1);
    ui->f_splitter1->restoreState(MSETTINGS->guiMainWindowLayout2());
}


void Simul6::writeSettings() {
    PDEBUG;
    MSETTINGS->setGuiMainWindowSize(size());
    MSETTINGS->setGuiMainWindowLayout1(saveState(1));
    MSETTINGS->setGuiMainWindowLayout2(ui->f_splitter1->saveState());
}

