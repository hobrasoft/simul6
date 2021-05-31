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
#include "ui_saveprogress.h"

#define SAVEPROGRESS SaveProgress::instance()

class Simul6;
namespace Ui {
class SaveProgress;
};

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
class SaveProgress : public QWidget {
    Q_OBJECT
  public:
    SaveProgress(QWidget *parent);
    enum Format { Csv = 1, Json, Sqlite3 };
    enum SaveMode { Swapped, Normal };
    static SaveProgress *instance();
   ~SaveProgress();

    const QString& filename() const { return m_filename; }
    bool   active() const { return m_active; }
    double interval() const { return ((double)m_interval)/1000.0; }
    Format format() const { return m_format; }
    void saveVA(double time, double voltage, double amperage);

  signals:
    void timeData(const QVariantMap& data);

  public slots:
    void slotTimeChanged(double);
    void slotFinished();
    void saveSwap();
    void init();

  private slots:
    void selectFile();
    void resetFile();
    void activeStateChanged();
    void showStepsForm();

  private:
    static SaveProgress *m_instance;
    QThread m_workerThread;
    SaveProgressWorker *m_worker;

    void saveTimeStep(double time);
    void saveJson(double time);
    void saveCsv(double time, SaveMode saveMode = Normal);
    void saveSqlite(double time);

    bool    m_active;
    bool    m_truncateVA;
    quint64 m_interval;
    quint64 m_savedTime;
    double  m_savedTimeReal;
    int     m_savedSteps;
    QString m_filename;
    Format  m_format;
    Simul6 *m_simul6;

    Db::Database *m_database;
    Ui::SaveProgress *ui;
};

#endif

