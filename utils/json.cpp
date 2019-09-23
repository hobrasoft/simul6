#include "json.h"
#include <QVariant>
#include <QList>
#include <QSet>
#include <QDebug>

#if QT_VERSION > 0x050000
#include <QJsonObject>
#include <QJsonParseError>
#else
#ifdef WIN32
#include <QJson/Parser>
#include <QJson/Serializer>
#else
#include <qjson/parser.h>
#include <qjson/serializer.h>
#endif
#endif


QByteArray JSON::json(const QVariant& data) {
    #if QT_VERSION > 0x050000
    QByteArray json = QJsonDocument::fromVariant(data).toJson(QJsonDocument::Indented);
    return json;

    #else

    bool ok = true;
    QJson::Serializer serializer;
    #ifdef ARM
    QByteArray json = serializer.serialize(data);
    #else
    QByteArray json = serializer.serialize(data, &ok);
    #endif
    if (!ok) {
        qDebug() << QString("Sorry, there is a problem with data serializing in JSON::json()");
        }

    return json;
    #endif
}


QByteArray JSON::json(const QVariant& data, bool *ok) {
    #if QT_VERSION > 0x050000
    QByteArray json = QJsonDocument::fromVariant(data).toJson(QJsonDocument::Indented);
    *ok = true;
    return json;

    #else

    *ok = true;
    QJson::Serializer serializer;
    #ifdef ARM
    QByteArray json = serializer.serialize(data);
    #else
    QByteArray json = serializer.serialize(data, ok);
    #endif
    if (!*ok) {
        qDebug() << QString("Sorry, there is a problem with data serializing in JSON::json()");
        }

    return json;
    #endif
    
}


QVariant JSON::data(const QByteArray& json) {
    #if QT_VERSION > 0x050000
    QJsonParseError error;
    QJsonDocument jdoc = QJsonDocument::fromJson(json, &error);
    return jdoc.toVariant();
    #else
    bool ok;
    QJson::Parser parser;
    return parser.parse(json, &ok);
    #endif
}


QVariant JSON::data(const QByteArray& json, bool *ok) {
    #if QT_VERSION > 0x050000
    QJsonParseError error;
    QJsonDocument jdoc = QJsonDocument::fromJson(json, &error);
    *ok = (error.error == QJsonParseError::NoError);
    return jdoc.toVariant();
    #else
    *ok = true;
    QJson::Parser parser;
    return parser.parse(json, ok);
    #endif
}

