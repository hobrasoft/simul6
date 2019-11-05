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

SaveProgress::SaveProgress(Simul6 *parent) : QObject(parent) {
    m_instance = this;
    m_simul6 = parent;
    m_format = Csv;
    m_interval = 10.0;
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
        m_filename = filename + ".csv";
        return;
        }

    if (m_format == Json && !m_filename.endsWith(".simul6.json", Qt::CaseInsensitive)) {
        m_filename = filename + ".simul6.json";
        return;
        }

}


void SaveProgress::setInterval(double interval) {
    m_interval = interval;
}


void SaveProgress::setActive(bool active) {
    m_active = active;
}


void SaveProgress::setFormat(Format format) {
    m_format = format;
}


void SaveProgress::init() {
    m_savedTime = -1000;
    m_data.clear();
    m_filename.clear();
    m_active = false;
    m_data = m_simul6->data();
}


void SaveProgress::slotFinished() {
    m_savedTime = -10000;
    slotTimeChanged(m_simul6->engine()->getTime());
}


void SaveProgress::slotTimeChanged(double time) {
    // PDEBUG << time << m_active << (m_savedTime + m_interval <= time);
    if (!m_active) { return; }
    if (m_savedTime + m_interval > time) {
        return; 
        }
    m_savedTime = time;

    if (m_format == Json) {
        saveJson(time);
        }
    if (m_format == Csv) {
        saveCsv(time);
        }
}


void SaveProgress::saveJson(double time) {
    // PDEBUG << time << "saving";

    const Engine *engine = m_simul6->engine();
    engine->lock();

    size_t p = engine->getNp(); // points

    QVariantList constituents;
    int id = 0;
    for (auto &sample : engine->getMix().getSamples()) {
        QVariantList concentrations;
        for (unsigned int i = 0; i <= p; i++){
            concentrations << sample.getA(0, i);
            }

        QVariantMap constituent;
        constituent["id"] = id;
        constituent["concentrations"] = concentrations;
        constituents << constituent;
        }

    engine->unlock();

    QVariantMap simulation;
    simulation["time"] = time;
    simulation["constituents"] = constituents;

    QVariantList simulations = m_data["simulation"].toList();
    simulations << simulation;
    m_data["simulation"] = simulations;

    QFile file(m_filename);
    if (!file.open(QIODevice::WriteOnly)) { return; }
    file.write(JSON::json(m_data));
    file.close();
}


void SaveProgress::saveCsv(double time) {
    QString timestamp = QString("%1").arg(time, 6, 'f', 2, QChar('0'));
    QString filename = m_filename;
    filename = filename.replace(QRegExp("\\.csv$", Qt::CaseInsensitive), timestamp+".csv");
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) { 
        PDEBUG << "Could not open" << filename;
        return; 
        }

    const Engine *engine = m_simul6->engine();
    engine->lock();
    double caplen = engine->getCapLen();
    size_t p = engine->getNp(); // points
    QStringList header;
    header << "\"length\"";
    header << "\"pH\"";
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
        pH = (pH > 0) ? -log(pH) / log(10) : 0;
        line << QString("%1").arg(len);
        line << QString("%1").arg(pH);
        for (unsigned int c = 0; c<constituentsCount; c++) {
            line << QString("%1").arg(engine->getMix().getSample(c).getA(0, i));
            }
        file.write(line.join(" ").toUtf8());
        file.write("\n");
        }
    
    file.close();
    engine->unlock();

}

