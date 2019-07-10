#include "pch.h"
#ifndef MIX_CPP
#define MIX_CPP

#include "Mix.h"
#include <iostream>

using namespace std;

Mix::Mix(const unsigned int pAreas, const int pNp) :
    areas(pAreas),
    np(pNp)
{
	cout << "Mix constructor" << endl;
}

Mix::~Mix()
{
	cout << "Mix desctuctor" << endl;
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

Sample &Mix::addConstituent(string pName, double pU, int pZ)
{
	Constituent c(pName, pU, pZ);

	return addConstituent(c);
}

vector<Sample>::iterator Mix::begin()
{
	return samples.begin();
}

vector<Sample>::iterator Mix::end()
{
	return samples.end();
}

void Mix::initV()
{
    for (auto &s : samples) {
        s.initV();
    }
}

void Mix::show()
{
	cout << "Showing mix" << endl;
	cout << "- total samples: " << samples.size() << endl;
	/*for (list<Sample *>::iterator it = samples.begin(); it !=samples.end(); it++) {
		(*it)->show();
	}*/
	for (vector<Sample>::iterator it = samples.begin(); it != samples.end(); it++) {
        it->show();
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
