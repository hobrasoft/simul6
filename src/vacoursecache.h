/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _VACourseCache_H_
#define _VACourseCache_H_

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QList>
#include <QPointF>

class Engine;

/**
 * @brief
 */
class VACourseCache : public QObject {
    Q_OBJECT
  public:
    VACourseCache();

   // do not declare these functions as slot, must not be called async
    void clear();
    void appendData(const Engine *);
    void lock() { m_mutex.lock(); }
    void unlock() { m_mutex.unlock(); }

    QList<QPointF> V() const { 
        QMutexLocker locker(&m_mutex);
        return m_V;
        }

    QList<QPointF> A() const { 
        QMutexLocker locker(&m_mutex);
        return m_A;
        }

  public slots:

  private:
    QList<QPointF> m_V;
    QList<QPointF> m_A;

    mutable QMutex  m_mutex;

};

#endif

