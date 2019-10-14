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

SaveProgress *SaveProgress::m_instance = nullptr;

SaveProgress::SaveProgress(Simul6 *parent) : QObject(parent) {
    m_instance = this;
    m_simul6 = parent;
    m_format = Json;
    m_interval = 1;
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
    m_savedTime= 0;
    m_data.clear();
    m_filename.clear();
    m_active = false;
    m_data = m_simul6->data();
}


void SaveProgress::slotTimeChanged(double time) {
    PDEBUG << time << m_active << (m_savedTime + m_interval <= time);
    if (!m_active) { return; }
    if (m_savedTime + m_interval > time) {
        return; 
        }
    m_savedTime = time;

    PDEBUG << time << "saving";

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
    if (!file.open(QIODevice::WriteOnly)) {
        SHOWMESSAGE(tr("Could not open or create file %1").arg(m_filename));
        return;
        }
    file.write(JSON::json(m_data));
    file.close();

}

