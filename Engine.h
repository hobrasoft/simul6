#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "Constituent.h"
#include "Sample.h"
#include "Mix.h"
#include "Vec2d.h"
//#include "Graph.h"
#include <list>
#include <string>
#include <vector>
#include <QObject>

using namespace std;

class Engine :  public QObject
{
    Q_OBJECT

public:
	/* Constants */
	static const double farc;
	static const double r;
	static const double te; // Temperature
	static const double w1;
	static const double w2;
	static const double w3;
	static const double w4;
	static const double alfa3;
    static const double beta2;
	static const double beta31;
	static const double beta32;
	static const double beta41;
	static const double beta42;
	static const double beta43;

    static const double W51;
    static const double W53;
    static const double W54;
    static const double W56;
    static const double W41;
    static const double W43;
    static const double W44;
    static const double W45;
    static const double W46;
    static const double C21;
    static const double C31;
    static const double C32;
    static const double C41;
    static const double C42;
    static const double C43;
    static const double C51;
    static const double C52;
    static const double C53;
    static const double C54;
    static const double C61;
    static const double C62;
    static const double C63;
    static const double C64;
    static const double C65;

    static const double uHpl; // Mobility of Hydrogen ions
    static const double uOHmin; // Mobility of Hydroxide ions
    static const double difHpl;
    static const double difOHmin;
    static const double pKw; // Ionic product of water
    static const double kw;
    static const unsigned int maxCharge; // Maximum negative and positive charge of compounds

    int np; // Number of axial intervals
    int bw, b1, b2, b3;
    int numb; // How often to draw
    double t; // Time
    double dx; // Lenght increment at x-axis [m]
    double dt; // Time increment [s]
    double curDen; // Current density [A/m2]
    double voltage;
    double capLen; // Physical length of capillary [m]
    double concUp; // Upper limit of y-axis [mmol/m3]
    double concDown; // Upper limit of y-axis [mmol/m3]
    double critG; // Criterion for ending iteration of G function
    double errL;  // Lovest error along x-axix
    double errH;  // Highest error along x-axix
    double errMax; // Max error along x-axix
    double timeInterval; // how often to display
    double timeDisplay; // time to display
    double timeStop;  // when to stop calculation
    int c0; // Transfer factor to SI system of units

    // 2-dimensional arrays
    //vector<vector<vector<double> > > a;
    //std::vector<std::vector<double> > d;
    //vector<vector<double> > v;

    /* Runge Kutta scope only */
    std::vector<std::vector<double> > q1;
    std::vector<std::vector<double> > q2;
    std::vector<std::vector<double> > q3;
    std::vector<std::vector<double> > q4;
    std::vector<std::vector<double> > q5;
    std::vector<std::vector<double> > q6;

    std::vector<std::vector<double> > pd1, pd2;

    // Vectors
    std::vector<double> dif;
    std::vector<double> hpl;  // Concentration of hydrogen ions
    std::vector<double> derHpl, derOH; // Used by der method only
    std::vector<double> kapa; // Conductivity
    std::vector<double> oH;
    std::vector<double> e;
    std::vector<double> error;
    std::vector<double> difPot;

    Mix mix;

    //Graph graph;
    Engine(unsigned int pAreas, int pNp);
    void setB(int pBw, int pB1, int pB2, int pB3);
	void setNumb(int pNumb) { numb = pNumb; }
	void setDx(double pDx) { dx = pDx; }
	void setT(double pT) { t = pT; }
	void setDt(double pDt) { dt = pDt; }
	void setCurDen(double pCurDen) { curDen = pCurDen; }
    void setVoltage(double pVoltage) { voltage = pVoltage; }
    void setCapLen(double pCapLen) { capLen = pCapLen; dx = capLen / np; }
    double getCapLen() { return capLen; }
    void setTimeStop(double pTimeStop) { timeStop = pTimeStop; }
    void setTimeInterval(double pTimeInterval) { timeInterval = pTimeInterval; }
    void setConcUp(double pConcUp) { concUp = pConcUp; }
    void setConcDown(double pConcDown) { concDown = pConcDown; }
    void setCritG(double pCritG) { critG = pCritG; }
    void setErrMax(double pErrMax) { errMax = pErrMax; }
    void setErrL(double pErrL) { errL = pErrL; }
    void setErrH(double pErrH) { errH = pErrH; }
    void setOptimizeDt(bool x) { m_optimizeDt = x; }

    void setTimeDisplay(double pTimeDisplay) {timeDisplay = pTimeDisplay; }


    void setC0(const int pC0) { c0 = pC0; }

    size_t getNm(); // Get number of constituents
    size_t getConstituentCount(); // Get number of constituents
    int getNp() { return np; }
	double getDx() { return dx; }
	Mix &getMix();
	void addMix(const Mix *m);

    void setup();
	void initArray(vector< vector<double> > &pVector);
	void initArrays();
    void initVectors();

    bool m_initialized;
    bool m_running;
    int  m_iterations;
    bool m_sendSignals;
    bool m_optimizeDt;

signals:
    //void drawGraph(Mix *pMix, std::vector<double> *pHpl);
    void drawGraph(Engine *pEngine, std::vector<double> *pHpl);
    void timeChanged(double);
    void errorChanged(double);
    void finished();
    void dtChanged(double);
    void curDenChanged(double);

public slots:
    void init();
    void run();
    void stop();

private slots:
    void runPrivate();

private:
	/* Computation */
    void gCalc();
	void der();
	void rungekutta();
    void cashkarp();

public:
	void show();

};

#endif
