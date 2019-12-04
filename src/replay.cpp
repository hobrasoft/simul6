/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "replay.h"
#include "Engine.h"
#include <QAction>
#include "pdebug.h"

Replay::~Replay() {
    delete ui;
}


Replay::Replay(QWidget *parent) : QWidget(parent) {
    ui = new Ui::Replay;
    ui->setupUi(this);
    m_replay = false;

    QAction *action;
    action = new QAction("⏮", this);
    ui->f_toBegin->setDefaultAction(action);
    connect(action, &QAction::triggered, this, &Replay::toBegin);

    action = new QAction("⏯", this);
    ui->f_step->setDefaultAction(action);
    connect(action, &QAction::triggered, this, &Replay::nextStep);

    action = new QAction("▶", this);
    ui->f_play->setDefaultAction(action);
    connect(action, &QAction::triggered, this, &Replay::play);

    action = new QAction("⏭", this);
    ui->f_toEnd->setDefaultAction(action);
    connect(action, &QAction::triggered, this, &Replay::toEnd);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &Replay::nextStep);

    connect(ui->f_slider, &QAbstractSlider::valueChanged, this, &Replay::setStep);
}


void Replay::clear() {
    PDEBUG;
    m_data.clear();
    m_step = 0;
    m_timer->stop();
    ui->f_slider->setRange(0, 0);
    setEnabled(false);
}


void Replay::setData(const QVariantList& data) {
    PDEBUG;
    setEnabled(true);
    m_replay = false;
    m_data = data;
    m_step = 0;
    ui->f_slider->setValue(0);
    ui->f_slider->setRange(0, data.size()-1);
    ui->f_step_number->setText("1");
    ui->f_size->setText(QString("%1").arg(data.size()-1));
}


void Replay::setEngine(Engine *engine) {
    PDEBUG;
    m_engine = engine;
}


void Replay::nextStep() {
    PDEBUG;
    m_step += 1;
    if (m_step >= m_data.size()) {
        m_step = m_data.size() - 1;
        m_replay = false;
        ui->f_toBegin->setEnabled(!m_replay);
        ui->f_toEnd->setEnabled(!m_replay);
        ui->f_slider->setEnabled(!m_replay);
        }
    ui->f_slider->setValue(m_step); // invokes setStep
}


void Replay::setStep(int step) {
    PDEBUG << step;
    if (step >= m_data.size()) {
        return;
        }
    m_step = step;
    ui->f_step_number->setText(QString("%1").arg(m_step));
    if (m_replay) {
        m_timer->setInterval(1000 / ui->f_speed->value());
        m_timer->start();
        }
    m_engine->setStep(m_data[m_step].toMap());
}


void Replay::toBegin() {
    PDEBUG;
    m_timer->stop();
    m_step = -1;
    nextStep();
}


void Replay::toEnd() {
    m_timer->stop();
    m_step = m_data.size() - 1;
    nextStep();
}


void Replay::play() {
    PDEBUG;
    if (m_replay) {
        m_replay = false;
        m_timer->stop();
      } else {
        m_replay = true;
        m_timer->start();
        }
    ui->f_toBegin->setEnabled(!m_replay);
    ui->f_toEnd->setEnabled(!m_replay);
    ui->f_slider->setEnabled(!m_replay);
}




