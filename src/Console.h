#pragma once
#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <vector>

using namespace std;

class Console
{
private:
	int width;
	int height;
	static const char ink = 'o';
	static const char paper = '.';

public:
	Console(int pWidth, int pHeight);
	void setXY(int pWidth, int pHeight);
	void normalizeXY(vector<double> &pArrayIn, vector<int> &pArrayOut);
	void normalizeXYZ(const vector<vector<double> > &pArray2dIn, vector<vector<int> > &pArray2dOut);
	void drawArray(vector<double> &pArray);
	void drawArray2d(const vector<vector<double> > &pArray2d);
	void drawSample(vector<vector<double> > &pArray2d, int pIndex);
	void drawSamples(vector<vector<double> > &pArray2d);
};

#endif
