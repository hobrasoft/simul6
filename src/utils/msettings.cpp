/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "msettings.h"
#include "pdebug.h"
#include <QCoreApplication>
#include <QStringList>
#include <QFileInfo>
#include <QDir>

MSettings *MSettings::m_instance = nullptr;


MSettings::~MSettings() {
    m_instance = nullptr;
}


MSettings::MSettings(QObject *parent) : QSettings(parent) {
    m_instance = this;
    setIniCodec("UTF-8");
}


void MSettings::setValue(const QString& key, const QVariant& value) {
    QSettings::setValue(key, value);
    sync();
}


QVariant MSettings::value(const QString& key, const QVariant& defaultValue) const {
    return QSettings::value(key, defaultValue);
}


/**
 * @brief Vytváří signleton instanci třídy, vyhledá konfigurační soubor
 */
MSettings *MSettings::instance(QObject *parent) {
    if (m_instance == nullptr) {
        Q_ASSERT(parent);
        new MSettings(parent);
        }
    return m_instance;
}


