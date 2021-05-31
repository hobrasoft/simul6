/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "exportprogresscsv.h"
#include "ui_exportprogresscsv.h"
#include "msettings.h"
#include "Engine.h"
#include "pdebug.h"
#include <math.h>
#include <QFileDialog>


ExportProgressCsv::~ExportProgressCsv() {
    m_engine->deleteLater();
}


ExportProgressCsv::ExportProgressCsv(QWidget *parent, int m_np) : QDialog(parent), ui(new Ui::ExportProgressCsv) {
    ui->setupUi(this);
    m_engine = new Engine(m_np);
    connect(ui->f_filename_select, &QAbstractButton::clicked, this, &ExportProgressCsv::selectFile);
}


void ExportProgressCsv::selectFile() {
    QString dirname = MSETTINGS->exportDirName();
    QString filename = QFileDialog::getSaveFileName(this, tr("Save simulation"), dirname,
          tr("Csv format (*.csv)")
        ).trimmed();
    if (filename.isEmpty()) { return; }
    m_filename = filename;
    ui->f_directory->setText(QFileInfo(filename).absoluteDir().absolutePath());
    ui->f_filename->setText(QFileInfo(filename).fileName());
}


void ExportProgressCsv::accept() {
    m_engine->init();
    for (int step = 0; step < m_data->size(); step++) {
        PDEBUG << "step" << step << m_data->size();
        const QVariantMap& stepData = m_data->step(step);
        const QVariantList& constituents = stepData["constituents"].toList();

        // If the constituent does not exist in engine yet,
        // add the constituent to engine.
        // This can happen when swap data are in steps data
        for (int i=0; i<constituents.size(); i++) {
            int internalId = constituents[i].toMap()["internal_id"].toInt();
            if (!m_engine->containsConstituentInternalId(internalId) && m_mixData.contains(internalId)) {
                const QVariantMap& cdata = m_mixData[internalId].toMap()["constituent"].toMap();
                m_engine->addConstituent(SegmentedConstituent(cdata));
                }
            }

        m_engine->setStep(stepData);
        saveCsv();
        }
    QDialog::accept();
}


void ExportProgressCsv::saveCsv() {
    double time = m_engine->getTime();
    QString filename = m_filename;
    QString timestamp = QString("%1").arg(time, 6, 'f', 2, QChar('0'));
//  if (saveMode == Swapped) { timestamp += "-swap"; }
    filename = filename.replace(QRegExp("\\.csv$", Qt::CaseInsensitive), timestamp+".csv");
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        // PDEBUG << "Could not open" << filename;
        return;
        }

    m_engine->lock();
    double caplen = m_engine->getCapLen();
    size_t p = m_engine->getNp(); // points
    QStringList header;
    header << "\"length\"";
    header << "\"pH\"";
    header << "\"cond\"";
    header << "\"Efield\"";
    for (auto &sample : m_engine->getMix().getSamples()) {
        header << "\""+sample.getName()+"\"";
        }
    file.write(header.join(" ").toUtf8());
    file.write("\n");

    unsigned int constituentsCount = m_engine->getMix().getSamples().size();
    for (unsigned int i = 0; i<= p; i++) {
        QStringList line;
        double len = caplen * ((double)i) / ((double)p);
        double pH = m_engine->getHpl()[i];
        double cond = m_engine->getKapa()[i]*1000.0;
        double efield = m_engine->getE()[i]/1000.0;
        efield = abs (efield);
        pH = (pH > 0) ? -log(pH) / log(10) : 0;
        line << QString("%1").arg(len);
        line << QString("%1").arg(pH);
        line << QString("%1").arg(cond);
        line << QString("%1").arg(efield);
        for (unsigned int c = 0; c<constituentsCount; c++) {
            line << QString("%1").arg(m_engine->getMix().getSample(c).getA(0, i));
            }
        file.write(line.join(" ").toUtf8());
        file.write("\n");
        }

    file.close();
    m_engine->unlock();
}




