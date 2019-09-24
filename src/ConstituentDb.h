#pragma once
#ifndef CONSTITUENTDB_HPP
#define CONSTITUENTDB_HPP

#include <QMap>
#include <QObject>
#include "Constituent.h"


class ConstituentDb : public QObject 
{
public:
	ConstituentDb(const QString& filename, QObject *parent = nullptr);
    ConstituentDb(QObject *parent = nullptr);
    ~ConstituentDb();

    const QMap<unsigned int, Constituent>& constituents() const { return m_constituents; }

private:
	QMap<unsigned int, Constituent> m_constituents;

private:
    void read();
    void read(const QString& filename);

public:
    size_t size();
	const Constituent &get(unsigned int pId);
};

#endif
