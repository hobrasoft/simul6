/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "database.h"
#include "msettings.h"
#include "msqlquery.h"
#include "pdebug.h"
#include <QFile>
#include <QSqlError>
#include <QUuid>

using namespace Db;

Database::Database(const QString& databasename, QObject *parent) : QObject(parent) {
    m_databasename = databasename;
}


bool Database::open() {
    if (m_isOpen) {
        close();
        }

    m_isOpen = false;
    PDEBUG << m_databasename;
    m_db = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString().toUtf8());
    if (!m_db.isValid()) {
        PDEBUG << "SQL database  driver is not valid";
        return false;
        }
    m_db.setDatabaseName ( m_databasename );
    m_db.open();
    if (m_db.isOpenError()) {
        PDEBUG << "SQL Database cannot be opened" << m_databasename;
        return false;
        }

    MSqlQuery q(m_db);
    q.exec("select * from version;");
    if (!q.next()) {
        PDEBUG << "Going to create SQL database";
        if (!create()) {
            return false;
            }
        }

    m_isOpen = true;
    q.exec("pragma foreign_key = ON;");

    emit opened(m_isOpen);
    return m_isOpen;
}


bool Database::close() {
    m_db.close();
    return true;
}


bool Database::create() {
    PDEBUG;
    QFile file(":/sqlite/sqlite.sql");
        if (!file.open(QIODevice::ReadOnly)) {
        PDEBUG << "Could not open SQL database file :/sqlite/sqlite.sql";
        return false;
        }

    while (!file.atEnd()) {
        QByteArray command;
        QByteArray line;
        do  {
            line = file.readLine();
            command += line;
            } while (!line.contains(";") && !file.atEnd());
        MSqlQuery q(m_db);
        if (command.isEmpty() || command.trimmed() == "") { continue; }
        if (!q.exec(QString::fromUtf8(command))) {
            QSqlError e = q.lastError();
            if (e.type() != QSqlError::NoError) {
                PDEBUG << q.lastQuery();
                PDEBUG << e.databaseText();
                PDEBUG << e.driverText();
                return false;
                }
            }
        }

    return true;
}


bool Database::upgrade() {
    MSqlQuery q(m_db);
    q.exec("select version from version;");
    q.next();
    int version = q.value(0).toInt();
    PDEBUG << version;
    for (;;) {
        version++;
        QFile file(QString(":/sqlite/patch.%1.sql").arg(version, 3, 10, QChar('0')));
        if (!file.open(QIODevice::ReadOnly)) {
            return true;
            }
        PDEBUG << version-1 << " -> " << version;

        while (!file.atEnd()) {
            QByteArray command;
            QByteArray line;
            do  {
                line = file.readLine();
                command += line;
                } while (!line.contains(";") && !file.atEnd());
            MSqlQuery q(m_db);
            if (!q.exec(QString::fromUtf8(command))) {
                QSqlError e = q.lastError();
                if (e.type() != QSqlError::NoError) {
                    PDEBUG << q.lastQuery();
                    PDEBUG << e.databaseText();
                    PDEBUG << e.driverText();
                    return false;
                    }
                }
            }

        q.prepare("update version set version = :version;");
        q.bindValue(":version", version);
        q.exec();
        }

    return true;
}



QString Database::dbVersion() const {
    QString version;
    MSqlQuery q(m_db);
    q.exec("select version from version;");
    if (q.next()) {
        version = q.value(0).toString();
        }
    return version;
}


QString Database::dbFileName() const {
    return m_databasename;
}


void Database::save(const Dbt::Steps& steps) {
    auto toString = [](const QList<double>& list) {
        QStringList stringlist;
        for (int i=0; i<list.size(); i++) {
            stringlist << QString("%1").arg(list[i]);
            }
        return stringlist.join(",");
        };
    MSqlQuery q(m_db);
    q.prepare("insert into steps (time, internal_id, values_array) values (:time, :internal_id, :values_array);");
    q.bindValue(":time", steps.time);
    q.bindValue(":internal_id", steps.internal_id);
    q.bindValue(":values_array", toString(steps.values_array));
    q.exec();
}


