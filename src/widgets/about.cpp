/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "ui_about.h"
#include "about.h"
#include "version.h"

About::~About() {
    delete ui;
}

About::About(QWidget *parent) : QDialog(parent), ui(new Ui::About) {
    ui->setupUi(this);
    ui->f_version->setText(VERSION);
}

