/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "replay.h"

Replay::~Replay() {
    delete ui;
}


Replay::Replay(QWidget *parent) : QWidget(parent) {
    ui = new Ui::Replay;
    ui->setupUi(this);
}


