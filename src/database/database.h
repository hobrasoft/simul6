/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _Database_H_
#define _Database_H_

#include <QObject>
#include <QSqlDatabase>
#include "dbt.h"

namespace Db {

class Database : public QObject {
    Q_OBJECT
  public:
    Database(const QString& databasename, QObject *parent);
    bool isOpen() const { return m_isOpen; }

  public slots:
    bool open();
    bool close();

    QString dbVersion() const;
    QString dbFileName() const;

    void    save(const Dbt::Steps&);

  signals:
    void    opened(bool);

  private:
    bool    m_isOpen;
    QSqlDatabase m_db;

    QString m_databasename;
    bool    create();
    bool    upgrade();

};

}

#endif

