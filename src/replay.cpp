/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "replay.h"
#include "Engine.h"
#include <QAction>
#include <QIcon>
#include "pdebug.h"
#include "simul6.h"
#include "graf.h"
#include "replaydatajson.h"
#include "replaydatasqlite3.h"

Replay::~Replay() {
    if (m_data != nullptr) { delete m_data; }
    delete ui;
}


Replay::Replay(QWidget *parent) : QWidget(parent) {
    ui = new Ui::Replay;
    ui->setupUi(this);
    m_replay = false;
    m_data = nullptr;

    QAction *action;
    action = new QAction(QIcon("://icons/begin.svg"), tr("Begin"), this);
    ui->f_toBegin->setDefaultAction(action);
    connect(action, &QAction::triggered, this, &Replay::toBegin);

    action = new QAction(QIcon("://icons/back.svg"), tr("Back"), this);
    ui->f_back->setDefaultAction(action);
    connect(action, &QAction::triggered, this, &Replay::prevStep);

    action = new QAction(QIcon("://icons/step.svg"), tr("Step"), this);
    ui->f_step->setDefaultAction(action);
    connect(action, &QAction::triggered, this, &Replay::nextStep);

    action = new QAction(QIcon("://icons/play.svg"), tr("Play"), this);
    ui->f_play->setDefaultAction(action);
    connect(action, &QAction::triggered, this, &Replay::play);
    m_actionPlay = action;

    action = new QAction(QIcon("://icons/end.svg"), tr("End"), this);
    ui->f_toEnd->setDefaultAction(action);
    connect(action, &QAction::triggered, this, &Replay::toEnd);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &Replay::nextStep);

    connect(ui->f_slider, &QAbstractSlider::valueChanged, this, &Replay::setStep);
}


void Replay::clear() {
    delete m_data;
    m_data = nullptr;
    m_step = 0;
    m_timer->stop();
    m_mixData.clear();
    ui->f_slider->setRange(0, 0);
    setEnabled(false);
}


void Replay::setData(const QString& databaseFilename) {
    clear();
    m_data = new ReplayDataSqlite3(databaseFilename);
    initReplay();
}

void Replay::setData(const QVariantList& data) {
    clear();
    m_data = new ReplayDataJson(data);
    initReplay();
}

void Replay::setMixData(const QVariantList& data) {
    for (int i=0; i<data.size(); i++) {
        int internalId = data[i].toMap()["constituent"].toMap()["internal_id"].toInt();
        PDEBUG << internalId << data[i];
        m_mixData[internalId] = data[i];
        }
}

void Replay::setSwapsData(const QVariantList& data) {
    PDEBUG << data;
    for (int i=0; i<data.size(); i++) {
        setMixData(data[i].toMap()["mix"].toList());
        }
}


void Replay::initReplay() {
    if (m_data == nullptr) { return; }
    setEnabled(true);
    m_replay = false;
    m_step = 0;
    ui->f_slider->setValue(0);
    ui->f_slider->setRange(0, m_data->size()-1);
    ui->f_step_number->setText("0");
    ui->f_size->setText(QString("%1").arg(m_data->size()-1));
    m_actionPlay->setIcon(QIcon("://icons/play.svg"));
}


void Replay::setEngine(Engine *engine) {
    m_engine = engine;
}


void Replay::nextStep() {
    if (m_data == nullptr) { return; }
    m_step += 1;
    if (m_step >= m_data->size()) {
        m_step = m_data->size() - 1;
        m_replay = false;
        ui->f_toBegin->setEnabled(!m_replay);
        ui->f_toEnd->setEnabled(!m_replay);
        ui->f_slider->setEnabled(!m_replay);
        ui->f_step->setEnabled(!m_replay);
        ui->f_back->setEnabled(!m_replay);
        m_actionPlay->setIcon(QIcon("://icons/play.svg"));
        }
    ui->f_slider->setValue(m_step); // invokes setStep
}


void Replay::prevStep() {
    if (m_data == nullptr) { return; }
    m_step -= 1;
    if (m_step < 0) {
        m_step = m_data->size() - 1;
        m_replay = false;
        ui->f_toBegin->setEnabled(!m_replay);
        ui->f_toEnd->setEnabled(!m_replay);
        ui->f_slider->setEnabled(!m_replay);
        ui->f_step->setEnabled(!m_replay);
        ui->f_back->setEnabled(!m_replay);
        m_actionPlay->setIcon(QIcon("://icons/play.svg"));
        }
    ui->f_slider->setValue(m_step); // invokes setStep
}


void Replay::setStep(int step) {
    if (m_data == nullptr) { return; }
    if (step >= m_data->size()) { return; }
    m_step = step;
    ui->f_step_number->setText(QString("%1").arg(m_step));
    if (m_replay) {
        m_timer->setInterval(1000 / ui->f_speed->value());
        m_timer->start();
        }
    Simul6::instance()->graf()->setRescaleEnabled(false);
    const QVariantMap& stepData = m_data->step(m_step);
    const QVariantList& constituents = stepData["constituents"].toList();

    // If the constituent does not exist in engine yet,
    // add the constituent to enginet.
    // This can happen when swap data are in steps data
    for (int i=0; i<constituents.size(); i++) {
        int internalId = constituents[i].toMap()["internal_id"].toInt();
        if (!m_engine->containsConstituentInternalId(internalId) && m_mixData.contains(internalId)) {
            const QVariantMap& cdata = m_mixData[internalId].toMap()["constituent"].toMap();
            m_engine->addConstituent(SegmentedConstituent(cdata));
            }
        }

    m_engine->setStep(stepData);
}


void Replay::toBegin() {
    if (m_data == nullptr) { return; }
    m_timer->stop();
    m_step = -1;
    nextStep();
}


void Replay::toEnd() {
    if (m_data == nullptr) { return; }
    m_timer->stop();
    m_step = m_data->size() - 1;
    nextStep();
}


void Replay::play() {
    if (m_replay) {
        m_replay = false;
        m_actionPlay->setIcon(QIcon("://icons/play.svg"));
        m_timer->stop();
      } else {
        m_replay = true;
        m_actionPlay->setIcon(QIcon("://icons/stop.svg"));
        m_timer->start();
        }
    ui->f_toBegin->setEnabled(!m_replay);
    ui->f_toEnd->setEnabled(!m_replay);
    ui->f_slider->setEnabled(!m_replay);
    ui->f_step->setEnabled(!m_replay);
    ui->f_back->setEnabled(!m_replay);
}




