/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "simulationprogressdialog.h"
#include "ui_simulationprogressdialog.h"
#include "saveprogress.h"
#include "msettings.h"
#include <QFileDialog>

SimulationProgressDialog::~SimulationProgressDialog() {
    delete ui;
}

SimulationProgressDialog::SimulationProgressDialog(QWidget *parent) : 
    QDialog(parent), 
    ui(new Ui::SimulationProgressDialog()) {
    ui->setupUi(this);
    ui->f_format_json->setChecked(true);
    ui->f_format_csv->setChecked(false);

    ui->f_filename->setText(SAVEPROGRESS->filename());
    ui->f_interval->setValue(SAVEPROGRESS->interval());
    ui->f_format_csv->setChecked((SAVEPROGRESS->format() == SaveProgress::Csv));
    ui->f_format_json->setChecked((SAVEPROGRESS->format() == SaveProgress::Json));

    connect(ui->f_filename_select, &QAbstractButton::clicked, this, &SimulationProgressDialog::selectFile);
    connect(ui->f_format_csv, &QAbstractButton::clicked, this, &SimulationProgressDialog::changeExtension);
    connect(ui->f_format_json, &QAbstractButton::clicked, this, &SimulationProgressDialog::changeExtension);
    connect(ui->f_filename, &QLineEdit::textChanged, [this](const QString& text) {
        ui->f_accept->setEnabled(!text.isEmpty());
        });
}


void SimulationProgressDialog::changeExtension() {
    QString filename = ui->f_filename->text();
    if (ui->f_format_csv->isChecked() && filename.endsWith(".csv", Qt::CaseInsensitive)) { return; }
    if (ui->f_format_json->isChecked() && filename.endsWith(".simul6.json", Qt::CaseInsensitive)) { return; }
    if (ui->f_format_csv->isChecked() && filename.endsWith(".simul6.json", Qt::CaseInsensitive)) { 
        filename = filename.replace(QRegExp("\\.simul6\\.json$", Qt::CaseInsensitive),".csv");
        ui->f_filename->setText(filename);
        return;
        }
    if (ui->f_format_json->isChecked() && filename.endsWith(".csv", Qt::CaseInsensitive)) { 
        filename = filename.replace(QRegExp("\\.csv$", Qt::CaseInsensitive),".simul6.json");
        ui->f_filename->setText(filename);
        return;
        }
}


void SimulationProgressDialog::selectFile() {
    QString dirname = MSETTINGS->dataDirName();
    QString filename = QFileDialog::getSaveFileName(this, tr("Save simulation"), dirname,
        (ui->f_format_json->isChecked())
            ? tr("Simul6 data [.simul6.json] (*.simul6.json);;csv (*.csv)")
            : tr("csv (*.csv);;Simul6 data [.simul6.json] (*.simul6.json)")
        );
    if (filename.isEmpty()) { return; }
    filename = filename.trimmed();
    MSETTINGS->setDataDirName(QFileInfo(filename).absoluteDir().absolutePath());

    if (!filename.endsWith(".simul6.json", Qt::CaseInsensitive) && ui->f_format_json->isChecked()) {
        filename = filename.replace(QRegExp("\\.+$"),"");
        filename += ".simul6.json";
        ui->f_filename->setText(filename);
        return;
        }

    if (!filename.endsWith(".csv", Qt::CaseInsensitive) && ui->f_format_csv->isChecked()) {
        filename = filename.replace(QRegExp("\\.+$"),"");
        filename += ".csv";
        ui->f_filename->setText(filename);
        return;
        }

    if (filename.endsWith(".simul6.json", Qt::CaseInsensitive)) {
        ui->f_format_json->setChecked(true);
        ui->f_format_csv->setChecked(false);
        ui->f_filename->setText(filename);
        return;
        }

    if (filename.endsWith(".csv", Qt::CaseInsensitive)) {
        ui->f_format_json->setChecked(false);
        ui->f_format_csv->setChecked(true);
        ui->f_filename->setText(filename);
        return;
        }

}


void SimulationProgressDialog::accept() {
    SAVEPROGRESS->setFilename(ui->f_filename->text());
    SAVEPROGRESS->setInterval(ui->f_interval->value());
    SAVEPROGRESS->setFormat((ui->f_format_csv->isChecked()) ? SaveProgress::Csv : SaveProgress::Json);
    QDialog::accept();
}


