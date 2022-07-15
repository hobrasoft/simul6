#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "Constituent.h"
#include "Sample.h"
#include "Mix.h"
#include "Vec2d.h"
#include "segmentedconstituent.h"
#include "crosssection.h"
#include "detectorcache.h"
#include <list>
#include <string>
#include <vector>
#include <QObject>
#include <QMutex>
#include <QDateTime>

using namespace std;

class DetectorCache;
class VACourseCache;

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
    static const double c0;
    static const double critG;
    static const unsigned int maxCharge; // Maximum negative and positive charge of compounds

    int np; // Number of axial grid points
    int bw; // bw thickness of the boundary between segments
    double t; // Time
    double dx; // Lenght increment at x-axis [m]
    double dt; // Time increment [s]
    double curDen; // Current density [A/m2]
    double voltage;
    double capLen; // Physical length of capillary [m]
    double errL;  // Lovest error along x-axix
    double errH;  // Highest error along x-axix
    double errMax; // Max error along x-axix
    double error;
    double timeInterval; // how often to display
    double timeDisplay; // time to display
    double timeStop;  // when to stop calculation
    QDateTime m_startTime;  // for measuring time elapsed
    QDateTime m_endTime;  // for measuring time elapsed
    QDateTime startTime() const { return m_startTime; }  // for measuring time elapsed
    QDateTime endTime() const { return m_endTime; }  // for measuring time elapsed


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
//    std::vector<double> error;
    std::vector<double> difPot;
    std::vector<double> cross;

    Mix mix;

    Engine(int pNp);

    double getTime() const { return t; }
	void setTime(double x) { t = x; emit timeChanged(x); timeChanged(this); }
	void setDx(double pDx) { dx = pDx; }
	void setT(double pT) { t = pT; }
	void setDt(double pDt) { dt = pDt; }
	void setCurDen(double pCurDen) { curDen = pCurDen; }
	double getCurDen() const { return curDen; }
    void setVoltage(double pVoltage) { voltage = pVoltage; }
    double getVoltage() const { return voltage; }
    void setConstantVoltage(bool x) { m_constantvoltage = x; }
    bool getConstantVoltage() const { return m_constantvoltage; }
    bool constantVoltage() const { return m_constantvoltage; }
    void setCapLen(double pCapLen) { capLen = pCapLen; dx = capLen / np; }
    double getCapLen() const { return capLen; }
    void setTimeStop(double pTimeStop) { timeStop = pTimeStop; }
    void setTimeInterval(double pTimeInterval);
    void setErrMax(double pErrMax) { errMax = pErrMax; }
    void setErrH(double pErrH) { errH = pErrH; errL = errH / 10.0; }
    void setOptimizeDt(bool x) { m_optimizeDt = x; }
    void setTimeDisplay(double pTimeDisplay) {timeDisplay = pTimeDisplay; }
    void setBW(int x) { bw = x; }
    int getBW() const { return bw; }
    bool containsConstituentInternalId(int internalId);
    void setDetectorActive(bool x) { m_detectorActive = x; }
    void setDetectorCache(DetectorCache *x);
    void setVACourseCache(VACourseCache *x);

    size_t getNm(); // Get number of constituents
    int getNp() const { return np; }
	double getDx() const { return dx; }
    const std::vector<double>& getHpl() const { return hpl; }
    const std::vector<double>& getKapa() const { return kapa; }
    const std::vector<double>& getE() const { return e; }
	const Mix &getMix() const;

    void setCrosssection(const Crosssection&); 
    void setMix(const QList<SegmentedConstituent>&);
    void addMix(const QList<SegmentedConstituent>&);
    void addConstituent(const SegmentedConstituent& constituent);
    void replaceConstituent(const SegmentedConstituent& constituent);
    void replaceConstituent(const SegmentedConstituent& newConstituent, Sample& replacedSample);
    void setStep(const QVariantMap& data);
	void initArray(vector< vector<double> > &pVector);
	void initArrays();
    void initVectors();
    void init();
    void reinit();

    bool m_initialized;
    bool m_running;
    bool m_sendSignals;
    bool m_optimizeDt;
    bool m_constantvoltage;
    bool m_detectorActive;

signals:
    //void drawGraph(Mix *pMix, std::vector<double> *pHpl);
    void drawGraph(const Engine *pEngine);
    void mixChanged(const Engine *pEngine);
    void timeChanged(double);
    void timeChanged(const Engine *pEngine);
    void errorChanged(double);
    void finished();
    void dtChanged(double);
    void curDenChanged(double);
    void voltageChanged(double);
    void replacedConstituent(int replacedInternalId, int replaceeInternalId);

public slots:
    void run();
    void stop();
    void lock() const;
    bool tryLock(int msec=5000) const;
    void unlock() const;
    void setVisible(int id, bool visible);

private slots:
    void runPrivate();

private:
	/* Computation */
    void gCalc();
	void der();
	void rungekutta();
    void cashkarp();
    mutable QMutex m_mutex;
    DetectorCache *m_detectorCache;
    VACourseCache *m_vacourseCache;
};

#endif
