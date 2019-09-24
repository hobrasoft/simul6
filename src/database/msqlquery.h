/**
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 * @file
 */
#ifndef _MSQLQUERY_H_
#define _MSQLQUERY_H_

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QString>
#include <QVariant>

namespace Db {

/**
 * @brief Vlastní rozšíření QSqlQuery o primitivní transakce a logování
 */
class MSqlQuery : public QSqlQuery {
  public:
   ~MSqlQuery();
    MSqlQuery();
    MSqlQuery(QSqlDatabase);

    bool exec (const QString& query);
    bool exec ();

    void begin();
    void rollback();
    bool commit();
    QString lastBoundQuery() const;

  private:
    void jeChyba();

    bool    m_inTransaction;
    bool    m_rollback;
    bool    m_success;
    bool    m_commit;

};

}

#endif
