/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "simulationprogressdialog.h"
#include "ui_simulationprogressdialog.h"
#include "saveprogress.h"
#include "msettings.h"
#include "pdebug.h"
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
    ui->f_format_sqlite->setChecked((SAVEPROGRESS->format() == SaveProgress::Sqlite3));
    ui->f_directory->setText(MSETTINGS->exportDirName());
    ui->f_accept->setEnabled(!ui->f_filename->text().isEmpty());

    connect(ui->f_filename_select, &QAbstractButton::clicked, this, &SimulationProgressDialog::selectFile);
    connect(ui->f_format_csv, &QAbstractButton::clicked, this, &SimulationProgressDialog::changeExtension);
    connect(ui->f_format_json, &QAbstractButton::clicked, this, &SimulationProgressDialog::changeExtension);
    connect(ui->f_format_sqlite, &QAbstractButton::clicked, this, &SimulationProgressDialog::changeExtension);
    connect(ui->f_filename, &QLineEdit::textChanged, [this](const QString& text) {
        ui->f_accept->setEnabled(!text.isEmpty());
        });
}


void SimulationProgressDialog::changeExtension() {
    QString filename = ui->f_filename->text();

    if (ui->f_format_csv->isChecked() && filename.endsWith(".csv", Qt::CaseInsensitive)) { return; }
    if (ui->f_format_json->isChecked() && filename.endsWith(".simul6.json", Qt::CaseInsensitive)) { return; }
    if (ui->f_format_sqlite->isChecked() && filename.endsWith(".simul6.sqlite3", Qt::CaseInsensitive)) { return; }

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

    if (ui->f_format_sqlite->isChecked() && filename.endsWith(".simul6.sqlite3", Qt::CaseInsensitive)) { 
        filename = filename.replace(QRegExp("\\.simul6\\.sqlite3$", Qt::CaseInsensitive),".simul6.sqlite3");
        ui->f_filename->setText(filename);
        return;
        }
}


void SimulationProgressDialog::selectFile() {
    QString dirname = MSETTINGS->exportDirName();
    QString filename = QFileDialog::getSaveFileName(this, tr("Save simulation"), dirname,
          (ui->f_format_json->isChecked())    ? tr("Simul6 data [.simul6.json] (*.simul6.json);;Simul6 data [.simul6.sqlite3] (*.simul6.sqlite3);;csv (*.csv)")
        : (ui->f_format_sqlite->isChecked())  ? tr("Simul6 data [.simul6.sqlite3] (*.simul6.json);;Simul6 data [.simul6.json] (*.simul6.json);;csv (*.csv)")
        :                                       tr("csv (*.csv);;Simul6 data [.simul6.sqlite3] (*.simul6.json);;Simul6 data [.simul6.json] (*.simul6.json)")
        );
    if (filename.isEmpty()) { return; }
    filename = filename.trimmed();
    ui->f_directory->setText(QFileInfo(filename).absoluteDir().absolutePath());
    MSETTINGS->setExportDirName(QFileInfo(filename).absoluteDir().absolutePath());

    if (!filename.endsWith(".simul6.json", Qt::CaseInsensitive) && ui->f_format_json->isChecked()) {
        filename = filename.replace(QRegExp("\\.+$"),"");
        filename += ".simul6.json";
        ui->f_filename->setText(filename);
        return;
        }

    if (!filename.endsWith(".simul6.sqlite3", Qt::CaseInsensitive) && ui->f_format_sqlite->isChecked()) {
        filename = filename.replace(QRegExp("\\.+$"),"");
        filename += ".simul6.sqlite3";
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
        ui->f_format_sqlite->setChecked(false);
        ui->f_format_csv->setChecked(false);
        ui->f_filename->setText(filename);
        return;
        }

    if (filename.endsWith(".csv", Qt::CaseInsensitive)) {
        ui->f_format_json->setChecked(false);
        ui->f_format_sqlite->setChecked(false);
        ui->f_format_csv->setChecked(true);
        ui->f_filename->setText(filename);
        return;
        }

    if (filename.endsWith(".simul6.sqlite3", Qt::CaseInsensitive)) {
        ui->f_format_json->setChecked(false);
        ui->f_format_sqlite->setChecked(true);
        ui->f_format_csv->setChecked(false);
        ui->f_filename->setText(filename);
        return;
        }

}


void SimulationProgressDialog::accept() {
    QString filename = QFileInfo(ui->f_filename->text().trimmed()).fileName();
    QString filepath = QDir(ui->f_directory->text()).absoluteFilePath(filename);
    // PDEBUG << filename << filepath << ui->f_directory->text();
    SAVEPROGRESS->setFormat(
        (ui->f_format_csv->isChecked()) ? SaveProgress::Csv : 
        (ui->f_format_json->isChecked()) ? SaveProgress::Json : 
                                            SaveProgress::Sqlite3
            ); // must be first!
    SAVEPROGRESS->setFilename(filepath);
    SAVEPROGRESS->setInterval(ui->f_interval->value());
    QDialog::accept();
}


