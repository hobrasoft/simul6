#include "computecontrol.h"
#include "ui_computecontrol.h"
#include "crosssectiondialog.h"
#include "pdebug.h"
#include <QTimer>
#include <omp.h>

ComputeControl::ComputeControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComputeControl)
{
    ui->setupUi(this);
    m_crosssectionModel = new CrosssectionModel(this);
    m_maxThreads = omp_get_max_threads();
    connect(ui->f_init, &QPushButton::clicked, this, &ComputeControl::init);
    connect(ui->f_run, &QPushButton::clicked, this, &ComputeControl::run);
    connect(ui->f_stop, &QPushButton::clicked, this, &ComputeControl::stop);
    connect(ui->f_run, &QPushButton::clicked, this, &ComputeControl::slotRunClicked);
    connect(ui->f_stop, &QPushButton::clicked, this, &ComputeControl::slotStopClicked);
    connect(ui->f_init, &QPushButton::clicked, this, &ComputeControl::slotInitClicked);
    connect(ui->f_parallel, &QAbstractButton::clicked, this, &ComputeControl::setParallelComputation);
    connect(ui->f_show_kapa, &QCheckBox::toggled, this, &ComputeControl::visibilityChangedKapa);
    connect(ui->f_show_ph, &QCheckBox::toggled, this, &ComputeControl::visibilityChangedPh);
    connect(ui->f_show_e, &QCheckBox::toggled, this, &ComputeControl::visibilityChangedE);
    connect(ui->f_caplen, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ComputeControl::caplenChanged);
    connect(ui->f_caplen, QOverload<double>::of(&QDoubleSpinBox::valueChanged), m_crosssectionModel, &CrosssectionModel::recalculate);
    setParallelComputation();
    QTimer::singleShot(0, this, [this](){
        m_crosssectionModel->recalculate();
        emit caplenChanged(ui->f_caplen->value());
        });
    connect(ui->f_crosssection, &QToolButton::clicked, [this]() {
        CrosssectionDialog d(this);
        d.exec();
        });
    connect(ui->f_diameter, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double x) {
        m_crosssectionModel->setDefaultDiameter(x);
        });
    connect(m_crosssectionModel, &QAbstractItemModel::dataChanged, this, &ComputeControl::enableOrDisableDiameter);
    connect(m_crosssectionModel, &QAbstractItemModel::columnsInserted, this, &ComputeControl::enableOrDisableDiameter);
    connect(m_crosssectionModel, &QAbstractItemModel::columnsRemoved, this, &ComputeControl::enableOrDisableDiameter);

}


void ComputeControl::enableOrDisableDiameter() {
    ui->f_diameter->setEnabled(m_crosssectionModel->unifiedDiameter());
    if (m_crosssectionModel->unifiedDiameter()) {
        ui->f_diameter->setValue(m_crosssectionModel->firstSegmentDiameter());
        }
}


ComputeControl::~ComputeControl()
{
    delete ui;
}


void ComputeControl::setKapaVisible(bool x) {
    ui->f_show_kapa->setChecked(x);
}


void ComputeControl::setPhVisible(bool x) {
    ui->f_show_ph->setChecked(x);
}


void ComputeControl::setEVisible(bool x) {
    ui->f_show_e->setChecked(x);
}


void ComputeControl::setParallelComputation() {
    if (ui->f_parallel->isChecked()) { 
        omp_set_num_threads(m_maxThreads);
      } else {
        omp_set_num_threads(1);
        }
}


void ComputeControl::showTime(double time) {
    ui->f_time->setText(QString("%1").arg(time, 0, 'f', 6));
}


void ComputeControl::showTimeElapsed(double time) {
    ui->f_time_elapsed->setText(QString("%1 s elapsed").arg(time, 0, 'f', 1));
}


bool ComputeControl::showKapa() const {
    return ui->f_show_kapa->isChecked();
}


bool ComputeControl::showPh() const {
    return ui->f_show_ph->isChecked();
}


bool ComputeControl::showE() const {
    return ui->f_show_e->isChecked();
}


void ComputeControl::initForm() {
    slotInitClicked();
}

void ComputeControl::slotInitClicked() {
    ui->f_init->setEnabled(true);
    ui->f_run->setEnabled(true);
    ui->f_stop->setEnabled(true);
}

void ComputeControl::slotRunClicked() {
    ui->f_init->setEnabled(false);
    ui->f_run->setEnabled(false);
    ui->f_stop->setEnabled(true);
}

void ComputeControl::slotStopClicked() {
    ui->f_init->setEnabled(true);
    ui->f_run->setEnabled(true);
    ui->f_stop->setEnabled(false);
}

void ComputeControl::slotFinished() {
    ui->f_init->setEnabled(true);
    ui->f_run->setEnabled(true);
    ui->f_stop->setEnabled(false);
}

double ComputeControl::getCapLen() const {
    return ui->f_caplen->value();
}

double ComputeControl::getTimeStop() const {
    return ui->f_timestop->value();
}

double ComputeControl::getTimeInterval() const {
    return ui->f_timeinterval->value();
}

int ComputeControl::getNp() const {
    return ui->f_np->value();
}

int ComputeControl::getBW() const {
    return (int)(((double)ui->f_np->value()) * ui->f_bw->value() / ui->f_caplen->value());
}

double ComputeControl::getBWmeters() const {
    return ui->f_bw->value() / 1000.0;
}

void   ComputeControl::setCaplen(double x) {
    ui->f_caplen->setValue(x);
}

void   ComputeControl::setBWmeters(double x) {
    ui->f_bw->setValue(x*1000.0);
}

void   ComputeControl::setNp(int x) {
    ui->f_np->setValue(x);
}

void   ComputeControl::setTimeInterval(double x) {
    ui->f_timeinterval->setValue(x);
}

void   ComputeControl::setTimeStop(double x) {
    ui->f_timestop->setValue(x);
}


