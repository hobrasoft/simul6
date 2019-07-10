#pragma once
#ifndef MIX_HPP
#define MIX_HPP

#include "Constituent.h"
#include "Sample.h"
#include <list>
#include <vector>
#include <string>

class Mix
{
private:
    std::vector<Sample> samples;
    unsigned int areas;
    int np;

public:
    Mix(const unsigned int pAreas, const int pNp);
	~Mix();
	Sample &addConstituent(const Constituent &pConstituent);
    Sample &addConstituent(const std::string pName, const double pU, const int pZ);
    Sample &getSample(unsigned int pIndex);
    std::vector<Sample> &getSamples() { return samples; }
    int getNp() { return np; }
    std::vector<Sample>::iterator begin();
    std::vector<Sample>::iterator end();
    void initV();

	void show();
    size_t size();
};

#endif
