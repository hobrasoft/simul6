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
#include "json.h"
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
    PDEBUG << data["time"];
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


SaveProgress::SaveProgress(Simul6 *parent) : QObject(parent) {
    m_instance = this;
    m_simul6 = parent;
    m_format = Csv;
    m_interval = 10000;
    m_workerThread.start();
    m_worker = new SaveProgressWorker();
    m_worker->moveToThread(&m_workerThread);
    connect(this, &SaveProgress::timeData, m_worker, &SaveProgressWorker::saveTimeData);

    init();

}


SaveProgress *SaveProgress::instance(Simul6 *parent) {
    if (m_instance == nullptr) {
        Q_ASSERT(parent != nullptr);
        new SaveProgress(parent);
        }
    return m_instance;
}


void SaveProgress::setFilename(const QString& filename) {
    m_filename = filename;

    if (m_format == Csv && !m_filename.endsWith(".csv", Qt::CaseInsensitive)) {
        m_filename = m_filename + ".csv";
        }

    if (m_format == Json && !m_filename.endsWith(".simul6.json", Qt::CaseInsensitive)) {
        m_filename = m_filename + ".simul6.json";
        }

    m_worker->setFilename(m_filename);

}


void SaveProgress::setInterval(double interval) {
    m_interval = 1000 * interval;
}


void SaveProgress::setActive(bool active) {
    m_active = active;
}


void SaveProgress::setFormat(Format format) {
    m_format = format;
}


void SaveProgress::init() {
    m_savedTime = 0;
    m_active = false;
    m_worker->setHeaderData(m_simul6->data());
}


void SaveProgress::slotFinished() {
    return;
    m_savedTime = 0;
    m_active = false;
    slotTimeChanged(m_simul6->engine()->getTime());
}


void SaveProgress::slotTimeChanged(double time) {
    if (!m_active) { return; }
    if (time > 0 && m_savedTime + m_interval > round(1000.0 * time)) {
        return; 
        }
    PDEBUG << m_savedTime << m_interval << (m_savedTime+m_interval) << time*1000.0 << (time > 0 && m_savedTime + m_interval > round(time*1000.0));

    if (m_format == Json) {
        saveJson(time);
        }
    if (m_format == Csv) {
        saveCsv(time);
        }

    if (time > 0) {
        m_savedTime += m_interval;
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
        constituent["internalId"] = sample.getInternalId();
        constituent["concentrations"] = concentrations;
        constituents << constituent;
        }

    engine->unlock();

    QVariantMap simulation;
    simulation["time"] = time;
    simulation["constituents"] = constituents;

    emit timeData(simulation);

}


void SaveProgress::saveCsv(double time) {
    QString timestamp = QString("%1").arg(time, 6, 'f', 2, QChar('0'));
    QString filename = m_filename;
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
        double cond = engine->getKapa()[i]*100.0;
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

