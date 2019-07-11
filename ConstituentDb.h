#pragma once
#ifndef CONSTITUENTDB_HPP
#define CONSTITUENTDB_HPP

#include <map>
#include "Constituent.h"

using namespace std;

class ConstituentDb
{
public:
    static const double uFactor;

public:
	ConstituentDb(const char *pFileName);
    ConstituentDb();
    ~ConstituentDb();

    const map<unsigned int, Constituent>& constituents() const { return m_constituents; }

private:
	map<unsigned int, Constituent> m_constituents;

private:
    void read();
    void read(const char *pFileName);

public:
	void show();
    size_t size();
	const Constituent &get(unsigned int pId);
};

#endif
