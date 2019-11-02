/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _SaveProgress_H_
#define _SaveProgress_H_

#include <QObject>
#include <QVariant>

#define SAVEPROGRESS SaveProgress::instance()

class Simul6;

/**
 * @brief
 */
class SaveProgress : public QObject {
    Q_OBJECT
  public:
    enum Format { Csv, Json };
    static SaveProgress *instance(Simul6 *parent = nullptr);

    const QString& filename() const { return m_filename; }
    bool   active() const { return m_active; }
    double interval() const { return m_interval; }
    Format format() const { return m_format; }

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

    void saveJson(double time);
    void saveCsv(double time);

    bool    m_active;
    QString m_filename;
    double  m_interval;
    double  m_savedTime;
    Format  m_format;
    QVariantMap m_data;
    Simul6 *m_simul6;
};

#endif

