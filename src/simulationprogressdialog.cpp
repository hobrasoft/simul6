/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "simulationprogressdialog.h"
#include "ui_simulationprogressdialog.h"

SimulationProgressDialog::~SimulationProgressDialog() {
    delete ui;
}

SimulationProgressDialog::SimulationProgressDialog(QWidget *parent) : 
    QDialog(parent), 
    ui(new Ui::SimulationProgressDialog()) {
    ui->setupUi(this);
}


