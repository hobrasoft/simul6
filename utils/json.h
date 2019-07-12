#ifndef _JSON_H_
#define _JSON_H_

#include <QVariant>
#include <QByteArray>
#include <QVariant>
#include <QList>
#include <QSet>

/**
 * Konverze mezi QVariant a JSON, pro zpětnou kompatibilitu mezi Qt4 a QJson a mezi Qt5
 *////
class JSON {
  public:

    /**
     * Převede strukturu v QVariant na JSON řetězec
     */
    static QByteArray json(const QVariant& data);               ///< Converts data to json

    /**
     * Převede strukturu v QVariant na JSON řetězec, chyby vrací v parametru ok
     */
    static QByteArray json(const QVariant& data, bool *ok);     ///< Converts data to json

    /**
     * Převede JSON řetězec na strukturu QVariant
     */
    static QVariant   data(const QByteArray& json);             ///< Converts json to data

    /**
     * Převede JSON řetězec na strukturu QVariant, chyby vrací v parametru ok
     */
    static QVariant   data(const QByteArray& json, bool *ok);   ///< Converts json to data
};

#endif
