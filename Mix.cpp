#include "pch.h"
#ifndef MIX_CPP
#define MIX_CPP

#include "Mix.h"

Mix::Mix(const unsigned int pAreas, const int pNp) :
    areas(pAreas),
    np(pNp)
{
	// cout << "Mix constructor" << endl;
}

Mix::~Mix()
{
	// cout << "Mix desctuctor" << endl;
	/*for (list<Sample *>::iterator it = samples.begin(); it !=samples.end(); it++) {
		delete *it;
	}*/
}

Sample &Mix::addConstituent(const Constituent &pConstituent)
{
	//Sample *s = new Sample(pConstituent, areas);
    Sample s(pConstituent, areas, np);
	samples.push_back(s);

	return samples.back();
}

Sample &Mix::addConstituent(const QString& pName, double pU, int pZ)
{
	Constituent c(pName, pU, pZ);

	return addConstituent(c);
}

std::vector<Sample>::iterator Mix::begin()
{
	return samples.begin();
}

std::vector<Sample>::iterator Mix::end()
{
	return samples.end();
}

void Mix::initV()
{
    for (auto &s : samples) {
        s.initV();
    }
}

size_t Mix::size()
{
    return samples.size();
}

Sample &Mix::getSample(unsigned int pIndex)
{
	return samples[pIndex];
}

#endif
