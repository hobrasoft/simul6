#include "msqlquery.h"
#include "pdebug.h"
#include <QSqlError>

using namespace Db;


MSqlQuery::~MSqlQuery() {
    if (!m_inTransaction) {
        return;
        }
    Q_ASSERT(m_commit == true || m_rollback == true);
}


MSqlQuery::MSqlQuery() : QSqlQuery() {
    m_inTransaction = false;
    m_rollback = false;
    m_success = true;
    m_commit = false;
    setForwardOnly(true);
}


MSqlQuery::MSqlQuery(QSqlDatabase db) : QSqlQuery(db) {
    m_inTransaction = false;
    m_rollback = false;
    m_success = true;
    m_commit = false;
    setForwardOnly(true);
}


void MSqlQuery::jeChyba() {
    m_success = false;
    QSqlError e = QSqlQuery::lastError();
    if (e.type() == QSqlError::NoError) {
        return;
        }
    QString text = lastBoundQuery();
    QString a = QString("%1\n%2\n%3\n") 
            .arg(e.databaseText())
            .arg(e.driverText())
            .arg(text)
            ;
    // PDEBUG << a << "\n" << stacktrace(16);
    // PDEBUG << a;
    if (m_inTransaction) { rollback(); }
}


QString MSqlQuery::lastBoundQuery() const {
    QString text = lastQuery();
    QMap<QString, QVariant> bvals = boundValues();
    QMapIterator<QString, QVariant> iterator(bvals);
    while (iterator.hasNext()) {
        iterator.next();
        QString key = iterator.key();
        QVariant val = bvals[key];
        switch ((QMetaType::Type)(val.type())) {
            case QMetaType::Int:
            case QMetaType::UInt:
            case QMetaType::Double:
            case QMetaType::Float:
            case QMetaType::Long:
            case QMetaType::LongLong:
            case QMetaType::Short:
            case QMetaType::UShort:
                text.replace(key, QString("%1").arg(val.toString()));
                break;
            default:
                text.replace(key, QString("'%1'").arg(val.toString()));
                break;
            };
        }
    return text;
}


bool MSqlQuery::exec (const QString& q) {
    Q_ASSERT(m_commit == false);
    Q_ASSERT(m_rollback == false);
    if (m_inTransaction && !m_success) { return false; }
    if (!QSqlQuery::exec(q)) { 
        jeChyba(); 
        return false; 
        }
    return true;
}


bool MSqlQuery::exec () {
    // Q_ASSERT(m_commit == false);
    // Q_ASSERT(m_rollback == false);
    if (m_inTransaction && !m_success) { return false; }
    if (!QSqlQuery::exec()) {
        jeChyba();
        return false;
        }
    return true;
}

void MSqlQuery::begin() {
    QSqlQuery::exec("begin;");
    m_commit = false;
    m_rollback = false;
    m_inTransaction = true;
}


void MSqlQuery::rollback() {
    Q_ASSERT(m_commit == false);
    Q_ASSERT(m_inTransaction == true);
    if (m_rollback) {
        return;
        }
    m_success = false;
    m_rollback = true;
    QSqlQuery::exec("rollback;");
}


bool MSqlQuery::commit() {
    // Q_ASSERT(m_rollback == false);
    Q_ASSERT(m_inTransaction == true);
    if (m_rollback) { return m_success; }
    if (m_success) {
        m_commit = true;
        QSqlQuery::exec("commit;");
        return m_success;
        }
    if (!m_success) {
        m_rollback = true;
        QSqlQuery::exec("rollback;");
        return m_success;
        }
    return m_success;
}

