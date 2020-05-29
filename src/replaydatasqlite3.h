/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _ReplayDataSqlite3_H_
#define _ReplayDataSqlite3_H_

#include "replaydataabstract.h" 

namespace Db {
class Database;
}

/**
 * @brief
 */
class ReplayDataSqlite3 : public ReplayDataAbstract {
  public:
    ReplayDataSqlite3(const QString& filename);
   ~ReplayDataSqlite3() Q_DECL_OVERRIDE;

    int size() const Q_DECL_OVERRIDE;
    QVariantMap step(int) const Q_DECL_OVERRIDE;

  private:
    Db::Database *m_db;
    QList<double> m_steps;
};

#endif

