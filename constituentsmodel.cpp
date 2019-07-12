#include "constituentsmodel.h"
#include "settings.h"
#include <Constituent.h>
#include <ConstituentDb.h>
#include <QTimer>
#include <QMap>
#include <QMapIterator>
#include <math.h>

ConstituentsModel::ConstituentsModel(QObject *parent)
    : QStandardItemModel(parent)
{
    createColumns();
    QTimer::singleShot(0, this, &ConstituentsModel::loadData);
}


void ConstituentsModel::createColumns() {
    insertColumns(0, LastCol);
    setHeaderData(Id, Qt::Horizontal, tr("Id"));
    setHeaderData(Name, Qt::Horizontal, tr("Name"));
    setHeaderData(NPka3, Qt::Horizontal, tr("-pKa 3"));
    setHeaderData(NPka2, Qt::Horizontal, tr("-pKa 2"));
    setHeaderData(NPka1, Qt::Horizontal, tr("-pKa 1"));
    setHeaderData(PPka1, Qt::Horizontal, tr("+pKa 1"));
    setHeaderData(PPka2, Qt::Horizontal, tr("+pKa 2"));
    setHeaderData(PPka3, Qt::Horizontal, tr("+pKa 3"));
    setHeaderData(NU3, Qt::Horizontal, tr("-u 3"));
    setHeaderData(NU2, Qt::Horizontal, tr("-u 2"));
    setHeaderData(NU1, Qt::Horizontal, tr("-u 1"));
    setHeaderData(PU1, Qt::Horizontal, tr("+u 1"));
    setHeaderData(PU2, Qt::Horizontal, tr("+u 2"));
    setHeaderData(PU3, Qt::Horizontal, tr("+u 3"));
}


void ConstituentsModel::loadData() {
    ConstituentDb db;
    QMap<unsigned int, Constituent> map = QMap<unsigned int, Constituent>(db.constituents());
    QMapIterator<unsigned int, Constituent> iterator(map);
    removeRows(0, rowCount());
    int row = 0;
    while (iterator.hasNext()) {
        iterator.next();
        unsigned int id = iterator.key();
        Constituent constituent = iterator.value();
        int posCount = constituent.getPosCount();
        int negCount = constituent.getNegCount();
        insertRows(row, 1);
        setData(index(row, Id), id);
        setData(index(row, Name), constituent.getName());
        if (posCount == 3) {
            setData(index(row, PPka3), (constituent.getPKa(+3)));
            setData(index(row, PU3), 10e8 * constituent.getU(+3));
        }
        if (negCount == 3) {
            setData(index(row, NPka3), (constituent.getPKa(-3)));
            setData(index(row, NU3), 10e8 * constituent.getU(-3));
        }
        if (posCount >= 2) {
            setData(index(row, PPka2), (constituent.getPKa(+2)));
            setData(index(row, PU2), 10e8 * constituent.getU(+2));
        }
        if (negCount >= 2) {
            setData(index(row, NPka2), (constituent.getPKa(-2)));
            setData(index(row, NU2), 10e8 * constituent.getU(-2));
        }
        if (posCount >= 1) {
            setData(index(row, PPka1), (constituent.getPKa(+1)));
            setData(index(row, PU1), 10e8 * constituent.getU(+1));
        }
        if (negCount >= 1) {
            setData(index(row, NPka1), (constituent.getPKa(-1)));
            setData(index(row, NU1), 10e8 * constituent.getU(-1));
        }
        row += 1;
    }
    emit loaded();
}

