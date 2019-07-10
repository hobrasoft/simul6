#pragma once
#ifndef CONSTITUENT_HPP
#define CONSTITUENT_HPP

#include <list>
#include <string>
#include <vector>

class Constituent
{
private:
	bool valid;
    std::string name;
	double U;
	int Z;
	unsigned int negCount;
	unsigned int posCount;
    std::vector<double> uNeg;
    std::vector<double> uPos;
    std::vector<double> pKaNeg;
    std::vector<double> pKaPos;
    std::vector<double> kaNeg;
    std::vector<double> kaPos;
    std::vector<double> lNeg; // do samplu?
    std::vector<double> lPos; // do samplu?
    double dif;

public:
	Constituent();
    Constituent(const std::string pName, const double pU = 0, const int pZ = 0);
    //Constituent(const Constituent &pConstituent);
	~Constituent();

	void setU(double pU) { U = pU; }
	void setZ(int pZ) { Z = pZ; }
	void addNegU(double pValue);
	void addPosU(double pValue);
	void addNegPKa(double pValue);
	void addPosPKa(double pValue);

    unsigned int getNegCount() { return negCount; }
    int getNegCharge() { return -1 * static_cast<int>(negCount); }
    unsigned int getPosCount() { return posCount; }
    int getPosCharge() { return static_cast<int>(posCount); }

    double getU() { return U; }
    double getU(int pCharge);
	int getZ() { return Z; }
    double getL(const int pCharge);


    double getPKa(const int pCharge);
    double getDif() { return dif; }
    void show();
    std::string &getName() { return name; }

private:
    void calculateDif();
};

#endif
