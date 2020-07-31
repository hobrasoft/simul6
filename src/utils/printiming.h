#ifndef _PRINTIMING_H_
#define _PRINTIMING_H_

#include <QDebug>
#include <QThread>
#include <QTime>

#define INITIMING \
    QTime inittime = QTime::currentTime(); \
    QTime relatime = QTime::currentTime();

#define PRINTIMING(x) \
    qDebug() << QThread::currentThread() << (x) << __LINE__ << inittime.msecsTo(QTime::currentTime()) << relatime.msecsTo(QTime::currentTime()); \
    relatime = QTime::currentTime();

#endif
