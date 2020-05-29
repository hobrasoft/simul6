/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _SaveProgress_H_
#define _SaveProgress_H_

#include <QObject>
#include <QVariant>
#include <QThread>
#include <QMutex>
#include <QTimer>
#include "database.h"

#define SAVEPROGRESS SaveProgress::instance()

class Simul6;

class SaveProgressWorker : public QObject {
    Q_OBJECT
  public:
    SaveProgressWorker();

    void setFilename(const QString& filename);
    void setHeaderData(const QVariantMap& data);

  signals:
    void saved();

  public slots:
    void saveTimeData(const QVariantMap& data);
    void stop();

  private slots:
    void save();

  private:
    QString m_filename;
    QVariantMap m_data;
    mutable QMutex  m_mutex;
    QTimer *m_timer;
    bool    m_nothingToSave;

};


/**
 * @brief
 */
class SaveProgress : public QObject {
    Q_OBJECT
  public:
    enum Format { Csv, Json, Sqlite3 };
    static SaveProgress *instance(Simul6 *parent = nullptr);
   ~SaveProgress();

    const QString& filename() const { return m_filename; }
    bool   active() const { return m_active; }
    double interval() const { return ((double)m_interval)/1000.0; }
    Format format() const { return m_format; }

  signals:
    void timeData(const QVariantMap& data);

  public slots:
    void slotTimeChanged(double);
    void slotFinished();
    void setFilename(const QString&);
    void setInterval(double);
    void setActive(bool);
    void setFormat(Format);
    void init();

  private:
    SaveProgress(Simul6 *parent);
    static SaveProgress *m_instance;
    QThread m_workerThread;
    SaveProgressWorker *m_worker;

    void saveJson(double time);
    void saveCsv(double time);
    void saveSqlite(double time);

    bool    m_active;
    quint64 m_interval;
    quint64 m_savedTime;
    QString m_filename;
    Format  m_format;
    Simul6 *m_simul6;

    Db::Database *m_database;
};

#endif

