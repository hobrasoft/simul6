/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "logger.h"
#include "msettings.h"
#include <QTextStream>
#include <QDateTime>
#include <QRegExp>
#include <QFile>

Logger *Logger::m_instance = nullptr;
QString Logger::m_log_include;
QString Logger::m_log_exclude;
bool    Logger::m_log_all;
QString Logger::m_log_file;


Logger::Logger(QObject *parent) : QObject (parent) {
    m_instance = this;
    m_log_all      = MSETTINGS->logAll();
    m_log_include  = MSETTINGS->logInclude();
    m_log_exclude  = MSETTINGS->logExclude();
    m_log_file     = MSETTINGS->logFile();

    qInstallMessageHandler(Logger::messageOutput);
}


Logger *Logger::instance(QObject *parent) {
    if (m_instance == nullptr) {
        Q_ASSERT(parent);
        new Logger(parent);
        }
    return m_instance;
}


void Logger::messageOutput(const QString& text) {
    Logger::messageOutput(QtDebugMsg, QMessageLogContext(), text);
}


void Logger::messageOutput(QtMsgType type, const QMessageLogContext&, const QString& text) {
    Q_UNUSED(type);
    QTextStream ostream(stderr);
    QFile file;
    if (!m_log_file.isEmpty()) {
        file.setFileName(m_log_file);
        if (file.open(QIODevice::Append)) {
            ostream.setDevice(&file);
            }
        }

    auto outstring = [&]() {
        ostream 
        << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
        << " "
        << text
        << "\r\n"
        ;
        };

    if (m_log_all && m_log_exclude.isEmpty()) {
        outstring();
        return;
        }

    if (m_log_all && !m_log_exclude.isEmpty()) {
        if (!text.contains(QRegExp(m_log_exclude))) {
            outstring();
            }
        return;
        }

    if (m_log_all) {
        outstring();
        return;
        }

    if (m_log_include.isEmpty()) {
        return;
        }

    if (!m_log_exclude.isEmpty() && text.contains(QRegExp(m_log_include))) {
        if (text.contains(QRegExp(m_log_exclude))) {
            return;
            }
        outstring();
        return;
        }

    if (text.contains(QRegExp(m_log_include))) {
        outstring();
        }

}


