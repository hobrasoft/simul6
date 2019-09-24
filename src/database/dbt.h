/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _DBT_H_
#define _DBT_H_

#include <QVariantList>
#include <QListIterator>
#include <QUuid>
#include <QDate>
#include <QDateTime>
#include <QCoreApplication>
#include "Constituent.h"
#include "segments.h"

template<typename T>
QVariantList toList(const QList<T>& input) {
    QVariantList list;
    QListIterator<T> iterator(input);
    while (iterator.hasNext()) {
        list << iterator.next().toMap();
        }
    return list;
}

namespace Dbt {

struct MixComponent {
    int         id;
    Constituent constituent;
    Segments    segments;
};

struct Control {
    QVariant    json;
};


struct Graf {
    double          time;
    int             id;     // references MixComponent(id)
    QList<double>   values_array;
};


}

#endif

