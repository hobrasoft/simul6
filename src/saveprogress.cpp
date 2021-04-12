/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "saveprogress.h"
#include "pdebug.h"
#include "simul6.h"
#include "Engine.h"
#include "messagedialog.h"
#include "msettings.h"
#include "replaydataabstract.h"
#include "json.h"
#include <QFileDialog>
#include <QMessageBox>
#include <math.h>

SaveProgress *SaveProgress::m_instance = nullptr;


SaveProgressWorker::SaveProgressWorker() : QObject() {
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(5000);
    connect(m_timer, &QTimer::timeout, this, &SaveProgressWorker::save);
    m_nothingToSave = true;
}


void SaveProgressWorker::saveTimeData(const QVariantMap& data) {
    // PDEBUG << qPrintable(JSON::json(data));
    QMutexLocker locker(&m_mutex);
    m_nothingToSave = false;

    QVariantList simulations = m_data["simulation"].toList();
    simulations << data;
    m_data["simulation"] = simulations;

    if (!m_timer->isActive()) { m_timer->start(); }
}


void SaveProgressWorker::stop() {
    QMetaObject::invokeMethod(this, "save", Qt::QueuedConnection);
    QEventLoop loop;
    connect(this, &SaveProgressWorker::saved, &loop, &QEventLoop::quit);
    loop.exec();
}


void SaveProgressWorker::setFilename(const QString& filename) {
    stop();
    QMutexLocker locker(&m_mutex);
    m_filename = filename;
}


void SaveProgressWorker::setHeaderData(const QVariantMap& data) {
    stop();
    QMutexLocker locker(&m_mutex);
    m_data = data;
}


void SaveProgressWorker::save() {
    // PDEBUG;
    QMutexLocker locker(&m_mutex);
    if (m_nothingToSave) { 
        // PDEBUG << "nothing to save";
        emit saved();
        return;
        }

    QFile file(m_filename);
    if (!file.open(QIODevice::WriteOnly)) { 
        emit saved();
        return; 
        }
    file.write(JSON::json(m_data));
    file.close();

    m_nothingToSave = true;
    emit saved();
    // PDEBUG << "complete";
}


SaveProgress::~SaveProgress() {
    m_workerThread.quit();
    m_workerThread.wait(500);
    m_workerThread.terminate();
    m_workerThread.wait(500);
    delete m_worker;
}


SaveProgress::SaveProgress(QWidget *parent) : QWidget (parent), ui(new Ui::SaveProgress) {
    ui->setupUi(this);
    m_instance = this;
    m_database = nullptr;
    m_simul6 = Simul6::instance();
    m_format = Csv;
    m_interval = 10000;
    m_workerThread.start();
    m_worker = new SaveProgressWorker();
    m_worker->moveToThread(&m_workerThread);
    connect(this, &SaveProgress::timeData, m_worker, &SaveProgressWorker::saveTimeData);

    ui->f_directory->setText(MSETTINGS->exportDirName());
    connect(ui->f_filename_select, &QAbstractButton::clicked, this, &SaveProgress::selectFile);
    connect(ui->f_active, &QCheckBox::stateChanged, this, &SaveProgress::activeStateChanged);
    connect(ui->f_filename_reset, &QAbstractButton::clicked, this, &SaveProgress::resetFile);

    QTimer::singleShot(0, this, &SaveProgress::init);

}


void SaveProgress::resetFile() {
    ui->f_active->setChecked(false);
    ui->f_active->setEnabled(false);
    ui->f_filename->setText(QString());
}


void SaveProgress::selectFile() {
    QString dirname = MSETTINGS->exportDirName();
    QString filename = QFileDialog::getSaveFileName(this, tr("Save simulation"), dirname,
          tr("Sqlite3 format (*.sqlite3);;"
             "Json format (*.json);;"
             "Csv format (*.csv)")
        ).trimmed();
    ui->f_active->setEnabled(!filename.isEmpty());
    if (filename.isEmpty()) { return; }
    m_filename = filename;
    ui->f_directory->setText(QFileInfo(filename).absoluteDir().absolutePath());
    MSETTINGS->setExportDirName(QFileInfo(filename).absoluteDir().absolutePath());
    ui->f_filename->setText(QFileInfo(filename).fileName());

    if (filename.endsWith(".json", Qt::CaseInsensitive))    { m_format = SaveProgress::Json; return; }
    if (filename.endsWith(".sqlite3", Qt::CaseInsensitive)) { m_format = SaveProgress::Sqlite3; return; }
    if (filename.endsWith(".csv", Qt::CaseInsensitive))     { m_format = SaveProgress::Csv; return; }

}


void SaveProgress::activeStateChanged() {
    bool active = ui->f_active->isChecked();
    ui->f_form->setEnabled(!active);
    // PDEBUG;

    if (active)  {
        m_active = true;
        m_interval = 1000 * ui->f_interval->value();

        const ReplayDataAbstract *replayData = Simul6::instance()->replayData();
        if (replayData != nullptr && replayData->size() > 0) {
            QMessageBox::warning(this,
                    tr("I have a warning"),
                    tr("The present record data will not be included"),
                    QMessageBox::Ok, QMessageBox::NoButton);

            if (m_database != nullptr) {
                m_database->close();
                delete m_database;
                m_database = nullptr;
                QFile::remove(m_filename);
                }

            const Engine *engine = m_simul6->engine();
            if (engine == nullptr) { return; }
            engine->lock();
            double time = engine->getTime();
            engine->unlock();
            if (time > 0) {
                m_savedTime = (quint64)(time*1000.0);
                m_savedTimeReal = time;
                m_savedSteps = 1;
                // PDEBUG << time;
                saveTimeStep(time);
              } else {
                init();
                QFile::remove(m_filename);
                m_active = true;
                }
          } else {
            init();
            QFile::remove(m_filename);
            m_active = true;
            }

        m_worker->setFilename(m_filename);

        showStepsForm();
        return;
        }

    if (!active) {
        m_active = false;
        init();
        showStepsForm();
        return;
        }

}

void SaveProgress::showStepsForm() {
    ui->f_active->setChecked(m_active);
    ui->f_savedsteps->setText(QString("%1").arg(m_savedSteps));
    ui->f_savedtime->setText(QString("%1").arg(m_savedTimeReal));
    ui->f_filesize->setText(QString("%1").arg(QFileInfo(m_filename).size()));
}



SaveProgress *SaveProgress::instance() {
    Q_ASSERT(m_instance != nullptr);
    return m_instance;
}


void SaveProgress::init() {
    // PDEBUG;
    if (m_database != nullptr) {
        m_database->close();
        delete m_database;
        }
    m_database = nullptr;
    m_savedTime = 0;
    m_savedTimeReal = 0;
    m_savedSteps = 0;
    m_active = false;
    m_worker->setHeaderData(m_simul6->data());
    showStepsForm();
}


void SaveProgress::slotFinished() {
    return;
}


void SaveProgress::slotTimeChanged(double time) {
    // PDEBUG << m_savedTime << time << m_interval << m_active;
    if (!m_active) { return; }
    if (time > 0 && m_savedTime + m_interval > round(1000.0 * time)) {
        return; 
        }
    saveTimeStep(time);
    m_savedTimeReal = time;
    m_savedSteps += 1;
    if (time > 0) {
        m_savedTime += m_interval;
        }
    showStepsForm();
}


void SaveProgress::saveTimeStep(double time) {
    // PDEBUG << m_savedTime << m_interval << (m_savedTime+m_interval) << time*1000.0 << (time > 0 && m_savedTime + m_interval > round(time*1000.0));

    if (m_format == Json) {
        saveJson(time);
        }
    if (m_format == Csv) {
        saveCsv(time);
        }
    if (m_format == Sqlite3) {
        saveSqlite(time);
        }

}


void SaveProgress::saveJson(double time) {
    // PDEBUG << time << "saving";

    const Engine *engine = m_simul6->engine();
    engine->lock();

    size_t p = engine->getNp(); // points

    QVariantList constituents;
    for (auto &sample : engine->getMix().getSamples()) {
        QVariantList concentrations;
        for (unsigned int i = 0; i <= p; i++){
            concentrations << sample.getA(0, i);
            }

        QVariantMap constituent;
        constituent["internal_id"] = sample.getInternalId();
        constituent["concentrations"] = concentrations;
        constituents << constituent;
        }

    engine->unlock();

    QVariantMap simulation;
    simulation["time"] = time;
    simulation["constituents"] = constituents;

    emit timeData(simulation);

}


void SaveProgress::saveSqlite(double time) {
    // PDEBUG << time;
    if (m_database == nullptr) {
        QFile::remove(m_filename);
        m_database = new Db::Database(m_filename, this);
        m_database->open();
        m_database->save(m_simul6->data());
        }

    if (m_database == nullptr) { 
        PDEBUG << "m_database == nullptr !!";
        return; 
        }
    if (!m_database->isOpen()) { 
        PDEBUG << "m_database->isOpen() == false !!";
        return; 
        }

    const Engine *engine = m_simul6->engine();
    engine->lock();
    size_t p = engine->getNp(); // points
    for (auto &sample : engine->getMix().getSamples()) {
        QList<double> list;
        for (unsigned int i = 0; i<=p; i++) {
            list << sample.getA(0,i);
            }

        Dbt::StepData stepdata;
        stepdata.time = time;
        stepdata.internal_id = sample.getInternalId();
        stepdata.values_array = list;
        m_database->save(stepdata);
        }
    
    engine->unlock();
    // PDEBUG << "saved";
}


void SaveProgress::saveCsv(double time, SaveMode saveMode) {
    QString filename = m_filename;
    QString timestamp = QString("%1").arg(time, 6, 'f', 2, QChar('0'));
    if (saveMode == Swapped) { timestamp += "-swap"; }
    filename = filename.replace(QRegExp("\\.csv$", Qt::CaseInsensitive), timestamp+".csv");
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) { 
        // PDEBUG << "Could not open" << filename;
        return; 
        }

    const Engine *engine = m_simul6->engine();
    engine->lock();
    double caplen = engine->getCapLen();
    size_t p = engine->getNp(); // points
    QStringList header;
    header << "\"length\"";
    header << "\"pH\"";
    header << "\"cond\"";
    header << "\"Efield\"";
    for (auto &sample : engine->getMix().getSamples()) {
        header << "\""+sample.getName()+"\"";
        }
    file.write(header.join(" ").toUtf8());
    file.write("\n");

    unsigned int constituentsCount = engine->getMix().getSamples().size();
    for (unsigned int i = 0; i<= p; i++) {
        QStringList line;
        double len = caplen * ((double)i) / ((double)p);
        double pH = engine->getHpl()[i];
        double cond = engine->getKapa()[i]*1000.0;
        double efield = engine->getE()[i]/1000.0;
        efield = abs (efield);
        pH = (pH > 0) ? -log(pH) / log(10) : 0;
        line << QString("%1").arg(len);
        line << QString("%1").arg(pH);
        line << QString("%1").arg(cond);
        line << QString("%1").arg(efield);
        for (unsigned int c = 0; c<constituentsCount; c++) {
            line << QString("%1").arg(engine->getMix().getSample(c).getA(0, i));
            }
        file.write(line.join(" ").toUtf8());
        file.write("\n");
        }
    
    file.close();
    engine->unlock();
}


void SaveProgress::saveSwap() {
    // PDEBUG;
    if (m_database != nullptr && m_format == Sqlite3) {
        // PDEBUG;
        m_database->save(m_simul6->data());		// Saves the JSON structure of basic mix + swap
        saveSqlite(m_savedTimeReal+0.000001);	// Saves computed data
        }

    if (m_format == Csv) {
        saveCsv(m_savedTimeReal, Swapped);
        }

}


