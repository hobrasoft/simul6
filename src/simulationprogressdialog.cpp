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

    ui->f_filename->setText(SAVEPROGRESS->filename());
    ui->f_interval->setValue(SAVEPROGRESS->interval());
    ui->f_directory->setText(MSETTINGS->exportDirName());
    ui->f_accept->setEnabled(!ui->f_filename->text().isEmpty());

    m_format = SaveProgress::Sqlite3;
    connect(ui->f_filename_select, &QAbstractButton::clicked, this, &SimulationProgressDialog::selectFile);
    connect(ui->f_filename, &QLineEdit::textChanged, [this](const QString& text) {
        ui->f_accept->setEnabled(!text.isEmpty());
        });
}


void SimulationProgressDialog::selectFile() {
    QString dirname = MSETTINGS->exportDirName();
    QString filename = QFileDialog::getSaveFileName(this, tr("Save simulation"), dirname,
          tr("Simul6 data, Sqlite3 format (*.simul6.sqlite3);;"
             "Simul6 data, Json format (*.simul6.json);;"
             "Csv format (*.csv)")
        ).trimmed();
    if (filename.isEmpty()) { return; }
    ui->f_filename->setText(filename);
    ui->f_directory->setText(QFileInfo(filename).absoluteDir().absolutePath());
    MSETTINGS->setExportDirName(QFileInfo(filename).absoluteDir().absolutePath());

    if (filename.endsWith(".simul6.json", Qt::CaseInsensitive))    { m_format = SaveProgress::Json; return; }
    if (filename.endsWith(".simul6.sqlite3", Qt::CaseInsensitive)) { m_format = SaveProgress::Sqlite3; return; }
    if (filename.endsWith(".csv", Qt::CaseInsensitive))            { m_format = SaveProgress::Csv; return; }

}


void SimulationProgressDialog::accept() {
    QString filename = QFileInfo(ui->f_filename->text().trimmed()).fileName();
    QString filepath = QDir(ui->f_directory->text()).absoluteFilePath(filename);
    SAVEPROGRESS->setFormat(m_format);
    SAVEPROGRESS->setFilename(filepath);
    SAVEPROGRESS->setInterval(ui->f_interval->value());
    QDialog::accept();
}


