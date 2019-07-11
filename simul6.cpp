#include "simul6.h"
#include "preferences.h"
#include "ui_simul6.h"
#include "computecontrol.h"
#include "simulationprofile.h"
#include "constituentsdialog.h"
#include <QSettings>
#include <QSize>
#include <QMessageBox>
#include <QTimer>

Simul6::Simul6(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Simul6)
{
    ui->setupUi(this);
    createActions();
    connect(ui->f_computeControl, &ComputeControl::init, ui->f_simulationProfile, &SimulationProfile::slotInit);
    connect(ui->f_computeControl, &ComputeControl::run, [this]() {
        ui->f_parameters->setEnabled(false);
        double current = ui->f_parameters->current();
        double dt = ui->f_parameters->dt();
        ui->f_simulationProfile->engine()->setCurDen(-508*current);
        ui->f_simulationProfile->engine()->setDt(dt);
        ui->f_simulationProfile->slotRun();
    });
    connect(ui->f_computeControl, &ComputeControl::stop, [this]() {
        ui->f_parameters->setEnabled(true);
        ui->f_simulationProfile->slotStop();
    });
    connect(ui->f_simulationProfile, &SimulationProfile::timeChanged, ui->f_computeControl, &ComputeControl::showTime);
    connect(ui->f_simulationProfile, &SimulationProfile::finished, [this]() {
        ui->f_parameters->setEnabled(true);
        ui->f_computeControl->slotFinished();
    });

    QTimer::singleShot(0, this, &Simul6::init);
}


void Simul6::init() {
    QSettings settings;
    QSize newsize = settings.value("windows/size", QSize(800,66)).toSize();
    resize(newsize);
}


void Simul6::createActions() {
    QMenu *menu = new QMenu(tr("Application"), this);
    QAction *action;
    action = new QAction(tr("Quit application"), this);
    menu->addAction(action);
    connect(action, &QAction::triggered, [this]() {
        zavrit();
    });

    action = new QAction(tr("Preferences"), this);
    connect(action, &QAction::triggered, [this]() {
        Preferences preferences(this);
        preferences.exec();
    });
    menu->addAction(action);

    ui->menuBar->addMenu(menu);
}


void Simul6::zavrit() {
    int rc= QMessageBox::question(this,
            tr("I have a question"),
            tr("Do you really want to close the application?"),
            QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);
    if (rc == QMessageBox::Yes) {
        QCoreApplication::quit();
    }
}


void Simul6::closeEvent(QCloseEvent *event) {
    zavrit();
    event->ignore();
}


Simul6::~Simul6()
{
    delete ui;
}
