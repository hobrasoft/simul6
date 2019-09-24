#include "pch.h"
#ifndef CONSTITUENTDB_CPP
#define CONSTITUENTDB_CPP

#include "ConstituentDb.h"
#include "messagedialog.h"
#include "json.h"

#include <QString>
#include <QByteArray>
#include <QFile>
#include "msettings.h"

using namespace std;

ConstituentDb::ConstituentDb(const QString& filename, QObject *parent) : QObject(parent)
{
    read(filename);
}

ConstituentDb::ConstituentDb(QObject *parent) : QObject(parent)
{
    read();
}

ConstituentDb::~ConstituentDb()
{
}

void ConstituentDb::read() {
    QString filename = MSETTINGS->dbFilename();
    read(filename);
}

void ConstituentDb::read(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        SHOWMESSAGE(tr("Could not open database file %1").arg(filename));
        return;
        }
    QByteArray text = file.readAll();
    file.close();

    QVariant data = JSON::data(text);
    
    QVariantList list = data.toMap()["constituents"].toList();
    QListIterator<QVariant> iterator(list);
    while (iterator.hasNext()) {
        const QVariantMap& item = iterator.next().toMap();
        int id = item["id"].toInt();
        Constituent constituent(item);
        constituent.setId(id);
        m_constituents[id] = constituent;

/*
        QString name = item["name"].toString();
        int negCount = item["negCount"].toInt();
        int posCount = item["posCount"].toInt();
        QVariantList uNeg = item["uNeg"].toList();
        QVariantList uPos = item["uPos"].toList();
        QVariantList pKaNeg = item["pKaNeg"].toList();
        QVariantList pKaPos = item["pKaPos"].toList();

        if (negCount != uNeg.size() || negCount != pKaNeg.size() ||
            posCount != uPos.size() || posCount != pKaPos.size()) {
            SHOWMESSAGE(tr("Bad format in database: id: %s  name: %s").arg(id).arg(name));
            continue;
            }

        Constituent constituent(name);
        constituent.setId(id);
        QListIterator<QVariant> citerator(uNeg);
        for (citerator = uNeg, citerator.toBack(); citerator.hasPrevious();) {
            constituent.addNegU(citerator.previous().toDouble() * Constituent::uFactor);
            }
        for (citerator = uPos; citerator.hasNext();) {
            constituent.addPosU(citerator.next().toDouble() * Constituent::uFactor);
            }
        for (citerator = pKaNeg, citerator.toBack(); citerator.hasPrevious();) {
            constituent.addNegPKa(citerator.previous().toDouble());
            }
        for (citerator = pKaPos; citerator.hasNext();) {
            constituent.addPosPKa(citerator.next().toDouble());
            }

        m_constituents[id] = constituent;
*/
        }

}

size_t ConstituentDb::size()
{
	return m_constituents.size();
}

const Constituent &ConstituentDb::get(unsigned int pId)
{
	return m_constituents[pId];
}

#endif
