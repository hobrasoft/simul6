#include "pch.h"
#ifndef ENGINE_CPP
#define ENGINE_CPP

#define _USE_MATH_DEFINES

#include "Engine.h"
#include "settings.h"
#include "pdebug.h"
#include "Constituent.h"
#include "ConstituentDb.h"
#include "Sample.h"
#include "messagedialog.h"
#include "detectorcache.h"
#include "Mix.h"
#include <iostream>
#include <cmath>
#include <chrono>
#include <QDebug>
#include <QTimer>

using namespace std;
//using namespace concurrency;

const double Engine::farc = 96487;
const double Engine::r = 8.314;
const double Engine::te = 298; // Temperature

const double Engine::w1 = 0.17476028;
const double Engine::w2 = 0.55148053;
const double Engine::w3 = 1.20553547;
const double Engine::w4 = 0.17118478;
const double Engine::beta2 = 0.4;
const double Engine::beta31 = 0.2969776;
const double Engine::beta32 = 0.15875966;
const double Engine::beta41 = 0.21810038;
const double Engine::beta42 = 3.0509647;
const double Engine::beta43 = 3.83286432;

const double Engine:: W51 = 37.0/378.0;
const double Engine:: W53 = 250.0/621.0;
const double Engine:: W54 = 125.0/594.0;
const double Engine:: W56 = 512.0/1771.0;
const double Engine:: W41 = 2825.0/27648.0;
const double Engine:: W43 = 18575.0/48384.0;
const double Engine:: W44 = 13525.0/55296.0;
const double Engine:: W45 = 277.0/14336.0;
const double Engine:: W46 = 0.25;
const double Engine:: C21 = 0.2;
const double Engine:: C31 = 3.0/40.0;
const double Engine:: C32 = 9.0/40.0;
const double Engine:: C41 =  0.3;
const double Engine:: C42 = -0.9;
const double Engine:: C43 =  1.2;
const double Engine:: C51 = -11.0/54.0;
const double Engine:: C52 =  2.5;
const double Engine:: C53 = -70.0/27.0;
const double Engine:: C54 =  35.0/27.0;
const double Engine:: C61 = 1631.0/55296.0;
const double Engine:: C62 = 175.0/512.0;
const double Engine:: C63 = 575.0/13824.0;
const double Engine:: C64 = 44275.0/110592.0;
const double Engine:: C65 = 253.0/4096.0;

const double Engine::uHpl = 363e-9;
const double Engine::uOHmin = 205e-9;
const double Engine::difHpl = Engine::uHpl * Engine::r * Engine::te/ Engine::farc;
const double Engine::difOHmin = Engine::uOHmin * Engine::r * Engine::te / Engine::farc;
const double Engine::pKw = 14;
const double Engine::c0 = 1000.0;
const double Engine::critG = 1e-7;
const double Engine::kw = exp(-Engine::pKw * log(10));

const unsigned int Engine::maxCharge = 3;


using namespace std;

Engine::Engine(int pNp) :
    np(pNp),
    t(0),
    errL(0),
    errH(0),
    errMax(0),
    m_initialized(false),
    m_running(false),
    m_sendSignals(false),    
    m_optimizeDt(false),
    m_detectorCache(nullptr)
{
    cout << "Engine constructor" << endl;
    QTimer *timer = new QTimer(this);
    timer->setInterval(500);
    timer->setSingleShot(false);
    timer->start();
    connect(timer, &QTimer::timeout, [this](){
        m_sendSignals = true;
    });
}


void Engine::setDetectorCache(DetectorCache *x) { 
    m_detectorCache = x; 
    x->clear(); 
}


const Mix &Engine::getMix() const 
{
    return mix;
}


size_t Engine::getNm()
{
    return mix.size();
}


void Engine::initArray(vector< vector<double> > &pVector)
{
    pVector.resize(getNp() + 1);

    for (int i = 0; i <= getNp(); ++i) {
        pVector[i].resize(getNm());
    }
}

void Engine::initArrays()
{
    //cout << "Initializing arrays" << endl;

    initArray(q1);
    initArray(q2);
    initArray(q3);
    initArray(q4);
    initArray(pd1);
    initArray(pd2);
}

void Engine::initVectors()
{
    //cout << "Initializing vectors for size: " << getNm() << " elements" << endl;
    hpl.resize(np + 1, 1e-16); // make the number as constant
    kapa.resize(np + 1, 0);
    oH.resize(np + 1, 0);
    e.resize(np + 1, 0);
    difPot.resize(np + 1, 0);
    cross.resize(np + 1, 1);
}


void Engine::addMix(const QList<SegmentedConstituent>& pconstituents) {
    Q_ASSERT(pconstituents.isEmpty() != true);
    const SegmentedConstituent& firstConstituent = pconstituents[0];
    int ratioSum = firstConstituent.ratioSum();

    // Vynuluje swap segment
    for (auto &sample : mix.getSamples()) {
        int segmentBegin = (int)((double)(np)/((double)ratioSum)*((double)firstConstituent.segments[0].ratio)) - bw/2;
        int segmentEnd   = (int)((double)(np)/((double)ratioSum)*((double)firstConstituent.segments[1].ratio)) + segmentBegin;
        double previousA = sample.getA(0, segmentBegin-1);
        double currentA  = 0;

        auto smooth = [this](double previous, double current, int segmentBegin, int i) {
            return (i < segmentBegin + bw)
                ? (previous + (current - previous) * (erf(-3 + static_cast<double>(i-segmentBegin) / bw * 6) + 1) / 2)
                : current;
            };

        // last segment must have length + 1
        if (segmentEnd >= np - 5) {
            segmentEnd = np + 1000;
            }
        for (int i = segmentBegin; i < segmentEnd && i <= np; i++) {
            if (i<0) { continue; }
            sample.setA(0, i, smooth(previousA, currentA, segmentBegin, i) );
            }

        segmentBegin = (int)((double)(np)/((double)ratioSum)*((double)firstConstituent.segments[0].ratio));
        segmentBegin = (int)((double)(np)/((double)ratioSum)*((double)firstConstituent.segments[1].ratio)) + segmentBegin;
        segmentEnd   = (int)((double)(np)/((double)ratioSum)*((double)firstConstituent.segments[2].ratio)) + segmentBegin;
        previousA = 0;
        currentA  = sample.getA(0, segmentBegin);

        if (segmentEnd + bw/2 >= np - 5) {
            segmentEnd = np + 1000;
            }
        for (int i = segmentBegin; i < segmentEnd && i <= np; i++) {
            if (i<0) { continue; }
            currentA = sample.getA(0, i);
            sample.setA(0, i, smooth(previousA, currentA, segmentBegin, i) );
            }

        }

    for (int row = 0; row < pconstituents.size(); row++) {
        if (mix.contains(pconstituents[row])) {
            replaceConstituent(pconstituents[row]);
          } else {
            addConstituent(pconstituents[row]);
            }
        }

    init();

}


void Engine::addConstituent(const SegmentedConstituent& constituent) {
    Sample sample(constituent, np);
    replaceConstituent(constituent, sample);
    mix.addSample(sample);
}


void Engine::replaceConstituent(const SegmentedConstituent& constituent) {
    Q_ASSERT(mix.contains(constituent));
    if (mix.contains(constituent)) {
        Sample& sample = mix.sample(constituent);   
        emit replacedConstituent(sample.getInternalId(), constituent.getInternalId());
        replaceConstituent(constituent, sample);
//      PDEBUG << "replacee" << constituent.getInternalId();
        return;
        }
    if (!mix.contains(constituent)) {
        addConstituent(constituent);
        return;
        }
}


void Engine::replaceConstituent(const SegmentedConstituent& constituent, Sample& sample) {
    struct ChargeValues {
        double value[7];
        double& operator[](int j) { return value[j+3]; }
        };

    int segmentsCount = constituent.size();
    int ratioSum = constituent.ratioSum();
    int negCharge = constituent.getNegCharge();
    int posCharge = constituent.getPosCharge();
    double prevConcentration = constituent.segments[0].concentration;
    double prevDif           = constituent.segments[0].constituent.getDif();
    ChargeValues prevL;
    ChargeValues prevU;
    for (int j=negCharge; j <= posCharge; j++) {
        prevL[j] = constituent.segments[0].constituent.getL(j);
        prevU[j] = constituent.segments[0].constituent.getU(j);
        }

    int segmentBegin = -bw/2;
    for (int segmentNumber = 0; segmentNumber < segmentsCount; segmentNumber++) {
        double concentration = constituent.segments[segmentNumber].concentration;
        double segmentRatio = constituent.segments[segmentNumber].ratio;
        double currentDif = constituent.segments[segmentNumber].constituent.getDif();
        ChargeValues currentL;
        ChargeValues currentU;
        for (int j = negCharge; j <= posCharge; j++) {
            if (j==0) { continue; }
            currentL[j] = constituent.segments[segmentNumber].constituent.getL(j);
            currentU[j] = constituent.segments[segmentNumber].constituent.getU(j);
            }

        int segmentEnd = segmentBegin + (int)((double)(np)/((double)ratioSum)*((double)segmentRatio));
        // last segment must have length + 1
        if (segmentEnd + bw/2 >= np - 5) {
            segmentEnd = np + 1;
            }

        // PDEBUG << "segments" << segmentBegin << segmentEnd;
        for (int i = segmentBegin; i < segmentEnd && i <= np; i++) {
            if (i<0) { continue; }
            auto smooth = [&segmentBegin,&i,this](double previous, double current) {
                return (i < segmentBegin + bw) 
                    ? (previous + (current - previous) * (erf(-3 + static_cast<double>(i-segmentBegin) / bw * 6) + 1) / 2) 
                    : current;
                };

            sample.setDif(   i, smooth(prevDif, currentDif));
            sample.addA  (0, i, smooth(prevConcentration, concentration));
            for (int j = negCharge; j <= posCharge; j++) {
                sample.setL  (j, i, smooth(prevL[j], currentL[j]));
                sample.setU  (j, i, smooth(prevU[j], currentU[j]));
                }

            // PDEBUG << i << sample.getA(0, i) << concentration;
            // PDEBUG << i << sample.getL(1, i) << sample.getU(1,i) << sample.getA(0, i);
            }

        //Here it should be expected filling u[j, i] according values in segments

        prevConcentration = concentration;
        prevDif = currentDif;
        prevU = currentU;
        prevL = currentL;
        segmentBegin = segmentEnd;
    }

}


void Engine::setMix(const QList<SegmentedConstituent>& pconstituents) 
{
    m_initialized = true;
    t = 0;

    // PDEBUG << pconstituents;

    initArrays();
    initVectors();

    for (int row = 0; row < pconstituents.size(); row++) {
        addConstituent(pconstituents[row]);
        }

}


void Engine::setCrosssection(const Crosssection& x) {
    int ratioSum = x.ratioSum();    
    const Crosssection::Section& firstSection = x[0];
    unsigned int segmentBegin = 0;

    double prevCross = 1e-12 * firstSection.diameter * firstSection.diameter * M_PI / 4;
    for (int segmentNumber = 0; segmentNumber < x.size(); segmentNumber++) {
        double diameter     = x[segmentNumber].diameter;
        double segmentRatio = x[segmentNumber].ratio;
        int segmentEnd      = segmentBegin + (int)((double)(np)/((double)ratioSum)*(segmentRatio));
        double currentCross = 1e-12 * diameter * diameter * M_PI / 4;

        if (segmentEnd >= np -5) { segmentEnd = np + 1; }

        for (unsigned int i = segmentBegin; i< segmentEnd && i <= np; i++) {
            auto smooth = [&segmentBegin,&i,this](double previous, double current) {
                return (i < segmentBegin + bw)
                    ? (previous + (current - previous) * (erf(-3 + static_cast<double>(i-segmentBegin) / bw * 6) + 1) / 2)
                    : current;
                };
            cross[i] = smooth(prevCross, currentCross);
            }
        prevCross = currentCross;
        segmentBegin = segmentEnd;
        }

}


void Engine::init() {
//Calculation of pH
   #pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++){
        hpl[i] = 1E-16;
    }
    gCalc();

//Calculation of conductivity
  #pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        double aV;
        aV = 0;
        oH[i] = kw / hpl[i];
        for (auto &s : mix.getSamples()) {
            for (int j = s.getNegCharge(); j <= s.getPosCharge(); j++) {
                    aV += s.getU(j, i) * s.getA(j, i) * abs(j);
                    // PDEBUG << i << j << s.getU(1,i) << s.getA(j,i);
                }
        }
        aV =(aV + c0 * uHpl * hpl[i] + c0 * uOHmin * oH[i]) * farc;
        kapa[i] = aV;

    }


//Calculation of current/voltage
    double Resist = 0;
    #pragma omp parallel for reduction(+:Resist)
    for (int i = 1; i <= np - 1; i++) Resist += (2+2*(i%2))/kapa[i]/cross[i];
    Resist += (1/kapa[0]/cross[0] + 1/kapa[np]/cross[np]);
    Resist *= (dx / 3);

    if (m_constantvoltage) {
        curDen = voltage / Resist;
    } else {
        voltage = curDen * Resist;
    }

    for (int i = 0; i <= np; i++) {
        e[i] = curDen / kapa[i] / cross[i];
    }


//Drawing
    emit mixChanged(this);
    emit drawGraph(this);
    emit curDenChanged(curDen);
    emit voltageChanged(voltage);
}


bool Engine::containsConstituentInternalId(int internalId) {
    for (auto &sample : mix.getSamples()) {
        if (internalId == sample.getInternalId()) { return true; }
        }
    return false;
}

/**
 * @brief Receives one step from data file, item "simulation"
 */
void Engine::setStep(const QVariantMap& data) {
    // PDEBUG;
    const QVariantList& constituents = data["constituents"].toList();
    if (constituents.isEmpty()) { return; }
    t = data["time"].toDouble();

    QSet<int> internalIdsToZero;
    for (auto &sample : mix.getSamples()) {
        internalIdsToZero << sample.getInternalId();
        }

    for (auto &sample : mix.getSamples()) {
        int internalId = sample.getInternalId();
        int sampleIndex = -1;
        for (int i=0; i<constituents.size(); i++) {
            if (constituents[i].toMap()["internal_id"].toInt() == internalId) {
                sampleIndex = i;
                break;
                }
            }
        if (sampleIndex < 0) { continue; }
        internalIdsToZero.remove(internalId);
        const QVariantMap& constituent = constituents[sampleIndex].toMap();
        const QVariantList& concentrations = constituent["concentrations"].toList();
        for (int i = 0; i<= np; i++) {
            sample.setA(0, i, concentrations[i].toDouble());
            }
        }

    for (auto &sample: mix.getSamples()) {
        int internalId = sample.getInternalId();
        if (!internalIdsToZero.contains(internalId)) { continue; }
        for (int i=0; i<=np; i++) {
            sample.setA(0, i, 0);
            }
        }

    init();
    emit timeChanged(t);
    m_detectorCache->appendData(this);
}

void Engine::gCalc()
{

#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        double hPlus, hPlusIn;

        hPlus = hpl[i];

        do {
            hPlusIn = hPlus;

            double hPlusPow [5];
            hPlusPow [0]=1;
            for (int j = 1; j<=4;j++){
                hPlusPow[j]=hPlusPow[j-1]*hPlus;
            }


            for (auto &s : mix.getSamples()) {

                double temp=1;
                for (int j = 1; j <= s.getPosCharge(); j++) {
                      temp+=s.getL(j, i) * hPlusPow[j];

                    }
                for (int j = -1; j >= s.getNegCharge(); j--) {
                        temp+=s.getL(j, i) / hPlusPow[-j];

                    }


                s.setH(0, 1 / temp, i);
                // PDEBUG << i << temp << hPlusPow[0] << hPlusPow[0] << hPlusPow[0] << hPlusPow[0] << hPlusPow[0] << s.getL(1, i) << s.getPosCharge() << s.getNegCharge();
                // PDEBUG << i << temp << s.getL(1, i) << s;
                for (int j = 1; j <= s.getPosCharge(); j++) {

                     s.setH(j, s.getL(j, i) * hPlusPow[j]/temp , i);
                    }
                for (int j = -1; j >= s.getNegCharge(); j--) {
                    s.setH(j, s.getL(j, i) / hPlusPow[-j]/temp , i);

                    }

                s.setHc(0, i);
                for (int j = s.getNegCharge(); j <= s.getPosCharge(); j++) {
                    s.addHc(s.getH(j, i) * j, i);
                }
            }

            // G
            double g = 0;
            for (auto &s : mix.getSamples()) {
                g += s.getHc(i) * s.getA(0, i);
            }
            g += hPlus * c0 - Engine::kw / hPlus * c0;


            // Der G
            for (auto &s : mix.getSamples()) {

                double temp = 0;
                for (int j = 1; j <= s.getPosCharge(); j++) {
                    temp-=s.getL(j, i) * hPlusPow[j-1] * j;

                    }
               for (int j = -1; j >= s.getNegCharge(); j--) {
                   temp-=s.getL(j, i) / hPlusPow[-j+1] * j;

                    }

               temp*=s.getH(0, i) * s.getH(0, i);
               s.setDerH(0, temp, i);
                for (int j = 1; j <= s.getPosCharge(); j++) {
                      s.setDerH(j, s.getL(j, i) * (temp * hPlusPow[j] + s.getH(0, i) * hPlusPow[j-1] * j), i);
                    }

                for (int j = -1; j >= s.getNegCharge(); j-- ) {
                    s.setDerH(j, s.getL(j, i) * (temp / hPlusPow[-j] + s.getH(0, i) / hPlusPow[-j+1] * j), i);
                    }

                s.setDerHc(0, i);

                for (int j = s.getNegCharge(); j <= s.getPosCharge(); j++) {
                    s.addDerHc(s.getDerH(j, i) * j, i);
                }
            }

            double derG = 0;
            for (auto &s : mix.getSamples()) {
                derG += s.getDerHc(i) * s.getA(0, i);
            }
            derG += c0 + Engine::kw / hPlusPow[2] * c0;
            // PDEBUG << i << derG;

            hPlus -= g / derG;

        } while (abs(hPlusIn - hPlus)/hPlus >= critG);

        // Calculation of charged species
        for (auto &s : mix.getSamples()) {
                for (int j = 1; j <= s.getPosCharge(); j++) {
                s.setA(j, i, s.getH(j, i) * s.getA(0, i));
            }

            for (int j = -1; j >= s.getNegCharge(); j--) {
                s.setA(j, i, s.getH(j, i) * s.getA(0, i));
            }
        }

        if (hPlus < 0 || std::isnan(hPlus) || std::isinf(hPlus)) {
            // PDEBUG << "hpl[i] < 0" << i << hPlus << t;
            }

        hpl[i] = hPlus;
    }

}


void Engine::der()
{
    double Resist;

#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        double aV;
        double aW;
        oH[i] = kw / hpl[i];
        aV = 0;
        aW = 0;

        for (auto &s : mix.getSamples()) {
            for (int j = s.getNegCharge(); j <= s.getPosCharge(); j++) {
                    double Aji = s.getA(j, i);
                    aV += s.getU(j,i) *Aji* abs(j);
                    aW += s.getDif(i) *Aji * j;
            }
        }

        aV =(aV + c0 * uHpl * hpl[i] + c0 * uOHmin * oH[i]) * farc;
        kapa[i] = aV;
        aW += c0 * hpl[i] * difHpl - c0 * oH[i] * difOHmin;
        difPot[i] = aW;
    }
/*end of pragma cycle*/


/*Simpson integration of resistance*/
    Resist = 0;
    for (int i = 1; i <= np - 1; i++) Resist += (2+2*(i%2))/kapa[i] / cross[i];
    Resist += (1/kapa[0]/cross[0] + 1/kapa[np]/cross[np]);
    Resist *= (dx / 3);

    if (m_constantvoltage) {
        curDen = voltage / Resist;
    } else {
        voltage = curDen * Resist;
    }

#pragma omp parallel for schedule(static)
    for (int i = 1; i <= np-1; i++) {
        double aV;
        e[i] = -(curDen/cross[i] + farc * (( -difPot[i - 1] + difPot[i + 1]) / 2 / dx)) / kapa[i];

        for (auto &s : mix.getSamples()) {
            aV = 0;
            for (int j = s.getNegCharge(); j <= s.getPosCharge(); j++) {
                int signj = (j > 0) - (j < 0);
                aV += s.getU(j,i) * s.getA(j, i) * signj;
            }
            s.setPd(i, aV * e[i]);
        }
    }
/*end of pragma cycle*/

    e[0] = e[1];
    e[np] = e[np - 1];
    for (auto &s : mix.getSamples()) {
        double aV = 0;
        double aW = 0;
        for (int j = s.getNegCharge(); j <= s.getPosCharge(); j++) {
                 int signj = (j > 0) - (j < 0);
                aV += s.getU(j,0) * s.getA(j, 0)  * signj;
                aW += s.getU(j,np) * s.getA(j, np) * signj;

        }
            s.setPd(0, aV * e[0]);
            s.setPd(np, aW * e[np]);
    }



#pragma omp parallel for schedule(static)
    for (int i = 1; i <= np - 1; i++) {
        for (auto &s : mix.getSamples()) {
            s.setD(0, i, (-s.getPd(i - 1)*cross[i - 1] + s.getPd(i + 1)*cross[i + 1]) / 2 / dx / cross[i]);
            s.addD(0, i, (s.getA(0, i - 1) * s.getDif(i) -2 * s.getA(0, i) * s.getDif(i) + s.getA(0, i + 1) * s.getDif(i)) / dx / dx);
        }
    }
for (auto &s : mix.getSamples()) {
         s.setD(0, 0, s.getD(0, 1));
         s.setD(0, np, s.getD(0, np - 1));
    }


/*  Alternative piece of code - double first derivatives.
 * It calculates the pH of strong unbuffered systems better,
 * however, it is less stable

#pragma omp parallel for schedule(static)
        for (int i = 1; i <= np - 1; i++) {
            for (auto &s : mix.getSamples()) {
                s.setD(0, i, (-s.getA(0, i - 1) + s.getA(0, i + 1)) * s.getDif(i) / 2 / dx);
            }
        }

        for (auto &s : mix.getSamples()) {
             s.setD(0, 0, s.getD(0, 1));
             s.setD(0, np, s.getD(0, np - 1));
        }

#pragma omp parallel for schedule(static)
        for (int i = 0; i <= np; i++) {
            for (auto &s : mix.getSamples()) {
                s.setPd(i, (s.getD(0, i) + s.getPd(i)));
            }
        }

#pragma omp parallel for schedule(static)
        for (int i = 1; i <= np - 1; i++) {
            for (auto &s : mix.getSamples()) {
                s.setD(0, i, (-s.getPd(i - 1) + s.getPd(i + 1)) / 2 / dx);
            }
        }
        for (auto &s : mix.getSamples()) {
             s.setD(0, 0, s.getD(0, 1));
             s.setD(0, np, s.getD(0, np - 1));
        }
*/

}

void Engine::rungekutta()
{
    mix.initV();

    der();

#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        for (auto &s : mix.getSamples()) {
            s.setQ1(i, s.getD(0, i) * dt);
            s.setA(0, i, s.getV(i) + beta2 * s.getQ1(i));
        }
    }

    gCalc();
    der();

#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        for (auto &s : mix.getSamples()) {
            s.setQ2(i, s.getD(0, i) * dt);
            s.setA(0, i, s.getV(i) + beta31 * s.getQ1(i) + beta32 * s.getQ2(i));
        }
    }
    gCalc();
    der();

#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        for (auto &s : mix.getSamples()) {
            s.setQ3(i, s.getD(0, i) * dt);
            s.setA(0, i, s.getV(i) + beta41 * s.getQ1(i) - beta42 * s.getQ2(i) + beta43 * s.getQ3(i));
        }
    }
    gCalc();
    der();

#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        for (auto &s : mix.getSamples()) {
            s.setQ4(i, s.getD(0, i) * dt);
            s.setA(0, i, s.getV(i) + w1 * s.getQ1(i) - w2 * s.getQ2(i) + w3 * s.getQ3(i) + w4 * s.getQ4(i));
        }
    }

    gCalc();
    t = t + dt;
}



void Engine::cashkarp()
{
    mix.initV();

    der();

#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        for (auto &s : mix.getSamples()) {
            s.setQ1(i, s.getD(0, i) * dt);
            s.setA(0, i, s.getV(i) + C21 * s.getQ1(i));
        }
    }

    gCalc();
    der();

#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        for (auto &s : mix.getSamples()) {
            s.setQ2(i, s.getD(0, i) * dt);
            s.setA(0, i, s.getV(i) + C31 * s.getQ1(i) + C32 * s.getQ2(i));
        }
    }
    gCalc();
    der();

#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        for (auto &s : mix.getSamples()) {
            s.setQ3(i, s.getD(0, i) * dt);
            s.setA(0, i, s.getV(i) + C41 * s.getQ1(i) + C42 * s.getQ2(i) + C43 * s.getQ3(i));
        }
    }
    gCalc();
    der();

#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        for (auto &s : mix.getSamples()) {
            s.setQ4(i, s.getD(0, i) * dt);
            s.setA(0, i, s.getV(i) + C51 * s.getQ1(i) + C52 * s.getQ2(i) + C53 * s.getQ3(i) + C54 * s.getQ4(i));
        }
    }
    gCalc();
    der();

#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        for (auto &s : mix.getSamples()) {
            s.setQ5(i, s.getD(0, i) * dt);
            s.setA(0, i, s.getV(i) + C61 * s.getQ1(i) + C62 * s.getQ2(i) + C63 * s.getQ3(i) + C64 * s.getQ4(i) + C65 * s.getQ5(i));
        }
    }
    gCalc();
    der();

    errMax = 0;

#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        for (auto &s : mix.getSamples()) {
            s.setQ6(i, s.getD(0, i) * dt);
            s.setA(0, i, s.getV(i) + W51 * s.getQ1(i) + W53 * s.getQ3(i) + W54 * s.getQ4(i) + W56 * s.getQ6(i));
        }

        error = 0;

        for (auto &s : mix.getSamples()) {
            error = error + s.getQ1(i)*(W51-W41) + s.getQ3(i)*(W53-W43)
                    + s.getQ4(i)*(W54-W44) - s.getQ5(i)*W45 + s.getQ6(i)*(W56-W46);
        }

        error = abs(error);

        if (error > errMax) errMax = error;
    }

    gCalc();

    if (errMax <= errL) dt = dt*1.1;
    else if (errMax > errH) dt = dt/1.5;

    t = t + dt;
}


void Engine::setVisible(int id, bool visible) {
    lock();
    mix.setVisible(id, visible);
    unlock();
}


void Engine::stop() {
    // PDEBUG;
    m_running = false;
}


void Engine::lock() const {
    m_mutex.lock();
}


void Engine::unlock() const {
    m_mutex.unlock();
}


void Engine::run()
{
    // PDEBUG;
    Q_ASSERT (m_initialized == true);
    m_running = true;
    if (t == 0) {
        emit timeChanged(t);
        m_detectorCache->appendData(this);
        }
    timeDisplay = timeInterval;
    intervalCounter = 0;
    QTimer::singleShot(0, this, &Engine::runPrivate);
}


void Engine::runPrivate() {
    if (!m_running || t > timeStop) {
        // PDEBUG;
        m_detectorCache->appendData(this);
        emit timeChanged(t);
        emit drawGraph(this);
        emit finished();
        emit timeElapsed(intervalCounter/1000);
        return;
    }

    lock();
    if (m_optimizeDt) {
        cashkarp();      // if Optimize dt is checked
    } else {
        rungekutta();    // if Optimize dt is not checked
    }
    unlock();
    emit timeChanged(t);
    m_detectorCache->appendData(this);

    if (t >= timeDisplay) {
        // qDebug() << "Engine::runPrivate()" << t;
        emit drawGraph(this);
        timeDisplay += timeInterval;
    }

    if (m_sendSignals) {
        emit errorChanged(errMax);
        emit dtChanged(dt);
        emit curDenChanged(curDen);
        emit voltageChanged(voltage);
        intervalCounter += 500;
        m_sendSignals = false;
    }

    QTimer::singleShot((m_detectorActive)?0:0, this, &Engine::runPrivate);
}

#endif
