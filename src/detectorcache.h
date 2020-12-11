/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _DetectorCache_H_
#define _DetectorCache_H_

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QList>
#include <QPointF>

class Engine;

/**
 * @brief
 */
class DetectorCache : public QObject {
    Q_OBJECT
  public:
    DetectorCache();

   // do not declare these functions as slot, must not be called async
    void clear();
    void appendData(const Engine *);
    void lock() { m_mutex.lock(); }
    void unlock() { m_mutex.unlock(); }

    QList<QList<QPointF> > data() const { 
        QMutexLocker locker(&m_mutex);
        return m_data;
        }

  public slots:
    void setDetectorPosition(double x) { m_detectorPosition = x; }

  private:
    QList<QList<QPointF> > m_data;

    mutable QMutex  m_mutex;
    double m_detectorPosition;
    

};

#endif

