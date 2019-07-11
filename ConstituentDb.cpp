#include "pch.h"
#ifndef CONSTITUENTDB_CPP
#define CONSTITUENTDB_CPP

#include "ConstituentDb.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <assert.h>
#include "json11.hpp"
#include <QString>
#include <QByteArray>
#include <QSettings>

using namespace std;
using namespace json11;

const double ConstituentDb::uFactor = 1e-9;

ConstituentDb::ConstituentDb(const char *pFileName)
{
    read(pFileName);
}

ConstituentDb::ConstituentDb()
{
    read();
}

ConstituentDb::~ConstituentDb()
{
}

void ConstituentDb::read() {
    QSettings settings;
    QString filename = settings.value("DB/filename").toString();
    read(filename.toUtf8().data());
}

void ConstituentDb::read(const char *pFileName)
{
    string err;
	ifstream ifs(pFileName);
	stringstream buffer;
	unsigned int key;

	buffer << ifs.rdbuf();
	auto json = Json::parse(buffer.str(), err);
	if (!err.empty()) {
        std::cout << "Failed: " << err.c_str() << std::endl;
	}
	else {
		for (auto &k : json["constituents"].array_items()) {
            key = static_cast<unsigned int>(k["id"].int_value());
			Constituent c(k["name"].string_value(), 0, 0);
            /*for (auto &l : k["uNeg"].array_items()) {
				c.addNegU(l.number_value());
			}
			for (auto &l : k["uPos"].array_items()) {
				c.addPosU(l.number_value());
			}
            for (auto &l : k["pKaNeg"].array_items()) {
				c.addNegPKa(l.number_value());
			}
            for (auto &l : k["pKaPos"].array_items()) {
				c.addPosPKa(l.number_value());
            }*/
            for (auto i = k["uNeg"].array_items().rbegin(); i != k["uNeg"].array_items().rend(); ++i) {
                c.addNegU(i->number_value() * uFactor);
            }

            for (auto i = k["uPos"].array_items().begin(); i != k["uPos"].array_items().end(); ++i) {
                c.addPosU(i->number_value() * uFactor);
            }

            for (auto i = k["pKaNeg"].array_items().rbegin(); i != k["pKaNeg"].array_items().rend(); ++i) {
                c.addNegPKa(i->number_value());
            }

            for (auto i = k["pKaPos"].array_items().begin(); i != k["pKaPos"].array_items().end(); ++i) {
                c.addPosPKa(i->number_value());
            }
            //c.show();
			m_constituents[key] = c;
		}

		cout << size() << " constituents read from DB" << endl;
	}
}

void ConstituentDb::show()
{
	cout << "Showing DB of constituents" << endl;
	for (auto &i : m_constituents) {
		cout << "Id: " << i.first << endl;
		i.second.show();
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
