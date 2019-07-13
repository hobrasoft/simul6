/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */
#ifndef PDEBUG

#include <QDebug>
#include <QRegExp>
#include <QString>
#include <QThread>

#define PDEBUG qDebug() \
            << QThread::currentThread() \
            << qPrintable(QString(__PRETTY_FUNCTION__) \
                    .replace(QRegExp("^[a-zA-Z*<>]*\\s"),"") \
                    .replace(QRegExp("\\(.*\\)"), "()") \
                    .replace(QRegExp("^void "), "") \
                    )

#endif
