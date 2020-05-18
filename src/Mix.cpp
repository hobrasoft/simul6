#include "pch.h"
#ifndef MIX_CPP
#define MIX_CPP

#include "Mix.h"

Mix::Mix() 
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


void Mix::setVisible(int id, bool visible) {
    for (auto &sample : samples) {
        if (sample.getInternalId() == id) {
            sample.setVisible(visible);
            }
        }
    
}


bool Mix::contains(const SegmentedConstituent& x) {
    for (auto &sample : samples) {
        if (sample.getName() == x.getName()) {
            return true;
            }
        }
    return false;
}


Sample &Mix::addSample(const Sample &sample)
{
	//Sample *s = new Sample(pConstituent, areas);
	samples.push_back(sample);

	return samples.back();
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

const Sample &Mix::getSample(unsigned int pIndex) const
{
	return samples[pIndex];
}

#endif
