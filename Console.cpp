#include "pch.h"
#ifndef CONSOLE_CPP
#define CONSOLE_CPP

#include "Console.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

using namespace std;

Console::Console(int pWidth, int pHeight)
	: width(pWidth), height(pHeight)
{
}

void Console::setXY(int pWidth, int pHeight)
{
	width = pWidth;
	height = pHeight;
}

void Console::normalizeXY(vector<double> &pArrayIn, vector<int> &pArrayOut)
{
	double myMax = 0;
    int size = static_cast<int>(pArrayIn.size());
    double myStep = static_cast<double>(size - 1) / static_cast<double>(width - 1);
	int index = 0;

	//cout << "Step: " << myStep << endl;

	for (double i = 0; i < size; i += myStep) {
		myMax = (pArrayIn[(int)i] > myMax ? pArrayIn[(int)i] : myMax);
	}
	//cout << "Max: " << myMax << endl;

	for (double i = 0; index < width; index++, i += myStep) {
		//cout << "i = " << i << ", round(i) = " << round(i) << ", index = " << index << endl;
		pArrayOut[index] = (int)(pArrayIn[(int)round(i)] / myMax * (height - 1));
	}
}

void Console::normalizeXYZ(const vector<vector<double >> &pArray2dIn, vector<vector<int> > &pArray2dOut)
{
	double myMax = 0;
	int size = (int) pArray2dIn.size();
	int sizeZ = (int) pArray2dIn[0].size();
	double myStep = (double)(size - 1) / (double)(width - 1);
	int index = 0;

	cout << "sizeZ: " << sizeZ << endl;
	cout << "Step: " << myStep << endl;

	for (double i = 0; i < size; i += myStep) {
		for (int k = 0; k < sizeZ; k++) {
			myMax = (pArray2dIn[(int)i][k] > myMax ? pArray2dIn[(int)i][k] : myMax);
		}
	}
	cout << "Max: " << myMax << endl;

	for (double i = 0; index < width; index++, i += myStep) {
		for (int k = 0; k < sizeZ; k++) {
			//cout << "i = " << i << ", round(i) = " << round(i) << ", index = " << index << endl;
			pArray2dOut[index][k] = (int)(pArray2dIn[(int)round(i)][k] / myMax * (height - 1));
		}
	}
}

void Console::drawArray(vector<double> &pArray)
{
	vector<int> outArray(width, 0);

	normalizeXY(pArray, outArray);

	for (int y = height - 1; y >= 0; y--) {
		cout << setw(3) << y << " ";
		for (int x = 0; x < width; x++) {
			if (outArray[x] == y) {
				cout << Console::ink;
			}
			else {
				cout << Console::paper;
			}
		}
		cout << endl;
	}
}

void Console::drawArray2d(const vector<vector<double> > &pArray2d)
{
	int sizeZ = (int)pArray2d[0].size();
	vector<vector<int> > outArray(width, vector<int>(sizeZ));

	normalizeXYZ(pArray2d, outArray);

	for (int y = height - 1; y >= 0; y--) {
		cout << setw(3) << y << " ";
		for (int x = 0; x < width; x++) {
			bool hit = false;
			for (int k = 0; k < sizeZ; k++) {
				if (outArray[x][k] == y) {
					cout << k;
					hit = true;
					break;
				}
			}
			if (!hit) {
				cout << Console::paper;
			}
		}
		cout << endl;
	}
}

void Console::drawSample(vector<vector<double> > &pArray2d, int pIndex)
{
	vector<double> array1d(pArray2d.size(), 0);

	for (unsigned int i = 0; i < pArray2d.size(); i++) {
		array1d[i] = pArray2d[i][pIndex];
	}

	drawArray(array1d);
}

void Console::drawSamples(vector<vector<double> > &pArray2d)
{
	drawArray2d(pArray2d);
}

#endif 
