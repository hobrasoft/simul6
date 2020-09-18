/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "replaydatasqlite3.h"
#include "database.h"
#include "pdebug.h"

ReplayDataSqlite3::~ReplayDataSqlite3() {
    m_db->close();
    delete m_db;
}


ReplayDataSqlite3::ReplayDataSqlite3(const QString& filename) : ReplayDataAbstract() {
    m_db = new Db::Database(filename);
    m_db->open();
    // PDEBUG << filename << "opened" << m_db->isOpen();
    m_steps = m_db->steps();
}


int ReplayDataSqlite3::size() const {
    return m_steps.size();
}


QVariantMap ReplayDataSqlite3::step(int x) const {
    if (x < 0 || x >= m_steps.size()) {
        return QVariantMap();
        }
    double time = m_steps[x];
    return m_db->stepData(time);
}

