#ifndef _Message_H_
#define _Message_H_

#include <QString>
#include <QCoreApplication>
#include <QDateTime>
#include <QObject>

#include "message.h"

class Message {
    Q_DECLARE_TR_FUNCTIONS(Message);
  public:
    Message();
    Message(const QDateTime&);
    Message(const QDateTime&, const QString& desc);
    Message(const QDateTime&, const QString& desc, const QString& msg);
    Message(                  const QString& desc);
    Message(                  const QString& desc, const QString& msg);

    QDateTime   time();
    QString     message();
    QString     description();

  private:
    QDateTime   m_time;
    QString     m_message;
    QString     m_description;
};


#endif
