#include "message.h"


Message::Message() {
    m_time = QDateTime::currentDateTime();
}


Message::Message(const QDateTime& t) {
    m_time = t;
}


Message::Message(const QDateTime& t, const QString& desc) {
    m_time = t;
    m_description = desc;
}


Message::Message(const QDateTime& t, const QString& desc, const QString& msg) {
    m_time = t;
    m_description = desc;
    m_message = msg;
}


Message::Message(const QString& desc) {
    m_time = QDateTime::currentDateTime();
    m_description = desc;
}


Message::Message(const QString& desc, const QString& msg) {
    m_time = QDateTime::currentDateTime();
    m_description = desc;
    m_message = msg;
}


QDateTime Message::time() {
    return m_time;
}


QString Message::message() {
    return m_message;
}


QString Message::description() {
    return m_description;
}

