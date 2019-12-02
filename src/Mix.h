#pragma once
#ifndef MIX_HPP
#define MIX_HPP

#include "Constituent.h"
#include "Sample.h"
#include <list>
#include <vector>
#include <QString>

class Mix
{
private:
    std::vector<Sample> samples;

public:
    Mix();
	~Mix();
	Sample &addSample(const Sample&);
    void setVisible(int id, bool visible);
    const Sample &getSample(unsigned int pIndex) const;
    const std::vector<Sample> &getSamples() const { return samples; }
    std::vector<Sample> &getSamples() { return samples; }
    std::vector<Sample>::iterator begin();
    std::vector<Sample>::iterator end();
    void initV();

    size_t size();
};

#endif
