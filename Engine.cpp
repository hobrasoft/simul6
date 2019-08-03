#include "pch.h"
#ifndef ENGINE_CPP
#define ENGINE_CPP

#include "Engine.h"
#include "settings.h"
#include "Constituent.h"
#include "ConstituentDb.h"
#include "Sample.h"
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

Engine::Engine(unsigned int pAreas, int pNp) :
    np(pNp),
    t(0),
    errL(0),
    errH(0),
    errMax(0),
    mix(pAreas, pNp),
    m_initialized(false),
    m_running(false),
    m_sendSignals(false),    
    m_optimizeDt(false)
{
    cout << "Engine constructor" << endl;
    QTimer *timer = new QTimer(this);
    timer->setInterval(500);
    timer->setSingleShot(false);
    timer->start();
    connect(timer, &QTimer::timeout, [this](){
        m_sendSignals = true;
    });
    setup();
}

void Engine::setB( int pBw, int pB1, int pB2, int pB3)
{
    bw = pBw;
    b1 = pB1;
    b2 = pB2;
    b3 = pB3;
}

Mix &Engine::getMix()
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
    //initArray(a);
    //initArray(d);
    //initArray(v);
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
    hpl.resize(np + 1, 1e-14); // make the number as constant
    derHpl.resize(np + 1, 0);
    derOH.resize(np + 1, 0);
    kapa.resize(np + 1, 0);
    oH.resize(np + 1, 0);
    e.resize(np + 1, 0);
    error.resize(np + 1);
    difPot.resize(np + 1, 0);
}

void Engine::init()
{
    qDebug() << "Engine::init()";
    m_initialized = true;
    t = 0;
    int i;

    initArrays();
    initVectors();

    for (auto &s : mix.getSamples()) {
        for (i = 0; i <= b1; i++) {
            s.setA(0, i, s.getIC(0));
        }
        for (i = b1 + 1; i <= b1 + bw; i++) {
            s.setA(0, i, s.getIC(0) + (s.getIC(1) - s.getIC(0)) * (erf(-2 + static_cast<double>(i - b1) / bw * 4) + 1) / 2);
        }
        for (i = b1 + bw + 1; i <= b2; i++) {
            s.setA(0, i, s.getIC(1));
        }
        for (i = b2 + 1; i <= b2 + bw; i++) {
            s.setA(0, i, s.getIC(1) + (s.getIC(2) - s.getIC(1)) * (erf(-2 + static_cast<double>(i - b2) / bw * 4) + 1) / 2);
        }
        for (i = b2 + bw + 1; i <= b3; i++) {
            s.setA(0, i, s.getIC(2));
        }
        for (i = b3 + 1; i <= b3 + bw; i++) {
            s.setA(0, i, s.getIC(2) + (s.getIC(3) - s.getIC(2)) * (erf(-2 + static_cast<double>(i - b3) / bw * 4) + 1) / 2);
        }
        for (i = b3 + bw + 1; i <= getNp(); i++) {
            s.setA(0, i, s.getIC(3));
        }
    }

    gCalc();
    //emit drawGraph(&mix, &hpl);
    emit drawGraph(this, &hpl);
    qDebug() << "Engine::init() hotovo";
}

void Engine::gCalc()
{

#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        double hPlus, hPlusIn;

        hPlus = hpl[i];

        do {
            hPlusIn = hPlus;

            for (auto &s : mix.getSamples()) {
                s.setH(0, 1, i);

                double hTemp = 1;
                for (int j = 1; j <= s.getPosCharge(); j++) {
                    hTemp = hTemp * hPlus;
                    s.addH(0, s.getL(j) * hTemp, i);
                }

                hTemp = 1;
                for (int j = -1; j >= s.getNegCharge(); j--) {
                    hTemp = hTemp * hPlus;
                    s.addH(0, s.getL(j) / hTemp, i);
                }

                s.setH(0, 1 / s.getH(0, i), i);
                hTemp = 1;
                for (int j = 1; j <= s.getPosCharge(); j++) {
                    hTemp = hTemp * hPlus;
                    s.setH(j, s.getH(0, i) * s.getL(j) * hTemp, i);
                }

                hTemp = 1;
                for (int j = -1; j >= s.getNegCharge(); j--) {
                    hTemp = hTemp * hPlus;
                    s.setH(j, s.getH(0, i) * s.getL(j) / hTemp, i);
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
                s.setDerH(0, 0, i);

                double hTemp = -1 / hPlus;
                for (int j = 1; j <= s.getPosCharge(); j++) {
                    hTemp = hTemp * hPlus;
                    s.addDerH(0, s.getL(j) * hTemp * j, i);
                }

                hTemp = -1 / hPlus;
                for (int j = -1; j >= s.getNegCharge(); j--) {
                    hTemp = hTemp / hPlus;
                    s.addDerH(0, s.getL(j) * hTemp * j, i);
                }

                s.setDerH(0, s.getDerH(0, i) * s.getH(0, i) * s.getH(0, i), i);

                hTemp = 1;
                for (int j = 1; j <= s.getPosCharge(); j++) {
                    hTemp = hTemp * hPlus;
                    s.setDerH(j, s.getL(j) * (s.getDerH(0, i) * hTemp + s.getH(0, i) * hTemp / hPlus * j), i);
                }

                hTemp = 1;
                for (int j = -1; j >= s.getNegCharge(); j-- ) {
                    hTemp = hTemp * hPlus;
                    s.setDerH(j, s.getL(j) * (s.getDerH(0, i) / hTemp + s.getH(0, i) / hTemp / hPlus * j), i);
                }

                s.setDerHc(0, i);

                for (int j = s.getNegCharge(); j <= s.getPosCharge(); j++) {
                    s.addDerHc(s.getDerH(j, i) * j, i);
                }
            }

            double derG = 0;
            for (unsigned int k = 0; k < getNm(); k++) {
                Sample &s = mix.getSample(k);
                derG += s.getDerHc(i) * s.getA(0, i);
            }
            derG += c0 + Engine::kw / hPlus / hPlus * c0;

            hPlus -= g / derG;
            if (g > 0) {
                //hPlus = hPlusIn / 1.1;
                //std::cout << "kuk" << endl;
            };

        } while (abs(hPlusIn - hPlus)/hPlus >= critG);

        // Calculation of charged species
        for (unsigned int k = 0; k < getNm(); k++) {
            Sample &s = mix.getSample(k);

            for (int j = 1; j <= s.getPosCharge(); j++) {
                s.setA(j, i, s.getH(j, i) * s.getA(0, i));
            }

            for (int j = -1; j >= s.getNegCharge(); j--) {
                s.setA(j, i, s.getH(j, i) * s.getA(0, i));
            }
        }

        hpl[i] = hPlus;
    }
/*konec pragmy*/

}

void Engine::der()
{
    double Resist;

#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        double aV;
        oH[i] = kw / hpl[i];
        aV = 0;

        for (auto &s : mix.getSamples()) {
            for (int j = s.getNegCharge(); j <= s.getPosCharge(); j++) {
                if (j != 0) {
                    aV += s.getU(j) * s.getA(j, i) * abs(j);
                }
            }
        }

        aV =(aV + c0 * uHpl * hpl[i] + c0 * uOHmin * oH[i]) * farc;
        kapa[i] = aV;

        aV = 0;

        for (auto &s : mix.getSamples()) {
            for (int j = s.getNegCharge(); j <= s.getPosCharge(); j++) {
                if (j != 0) {
                    aV += s.getDif() * s.getA(j, i) * j;
                }
            }

            aV += c0 * hpl[i] * difHpl - c0 * oH[i] * difOHmin;
            difPot[i] = aV;
        }
    }
/*konec pragmy*/


    Resist = 0;
    for (int i = 1; i <= np/2 - 1; i++) Resist = Resist + 2/kapa[2*i];
    for (int i = 1; i <= np/2; i++) Resist = Resist + 4/kapa[2*i - 1];
    Resist = Resist + 1/kapa[0] + 1/kapa[np];
    Resist = Resist * dx / 3;
    curDen = voltage / Resist;

    for (int i = 1; i <= np - 1; i++) {
        e[i] = ( -difPot[i - 1] + difPot[i + 1]) / 2 / dx;
    }

    e[0] = e[1];
    e[np] = e[np - 1];


#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        double aV;
        e[i] = (curDen - farc * e[i]) / kapa[i];

        for (auto &s : mix.getSamples()) {
            aV = 0;
            for (int j = s.getNegCharge(); j <= s.getPosCharge(); j++) {
                if (j != 0) {
                    aV += s.getU(j) * s.getA(j, i) * j / abs(j);
                }
            }
            s.setPd(i, aV * e[i]);
        }
    }
/*konec pragmy*/


    for (auto &s : mix.getSamples()) {
         for (int i = 1; i <= np - 1; i++) {
            s.setD(0, i, (-s.getPd(i - 1) + s.getPd(i + 1)) / 2 / dx);
         }
         s.setD(0, 0, s.getD(0, 1));
         s.setD(0, np, s.getD(0, np - 1));

         for (int i = 1; i <= np - 1; i++) {
            s.addD(0, i, (s.getA(0, i - 1) * s.getDif() -2 * s.getA(0, i) * s.getDif() + s.getA(0, i + 1) * s.getDif()) / dx / dx);
         }
         s.setD(0, 0, s.getD(0, 1));
         s.setD(0, np, s.getD(0, np - 1));
    }
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

// tahle srandicka je pro vyzkouseni jak funguje omp parallel
// #pragma omp parallel for schedule(static)
//        for (int i = 0; i <= np; i++) double kuk = 1.0;
//

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

        error[i] = 0;

        for (auto &s : mix.getSamples()) {
            error[i] = error[i] + s.getQ1(i)*(W51-W41) + s.getQ3(i)*(W53-W43)
                    + s.getQ4(i)*(W54-W44) - s.getQ5(i)*W45 + s.getQ6(i)*(W56-W46);
        }

        error[i] = abs(error[i]);

        if (error[i] > errMax) errMax = error[i];
    }

    gCalc();

    if (errMax <= errL) dt = dt*1.1;
    else if (errMax > errH) dt = dt/1.1;

    t = t + dt;
}


void Engine::stop() {
    qDebug() << "Engine::stop()";
    m_running = false;
}


void Engine::run()
{
    qDebug() << "Engine::run()";
    if (!m_initialized) {
        init();     
    }
    timeDisplay = timeInterval;
    intervalCounter = 0;
    m_running = true;
    QTimer::singleShot(0, this, &Engine::runPrivate);
}

void Engine::runPrivate() {

    if (!m_running || t >= timeStop) {
        emit drawGraph(this, &hpl);
        emit timeChanged(t);
        emit finished();
        qDebug() << "Time elapsed " << intervalCounter / 1000 << " s";
        return;
    }

    if (m_optimizeDt) {
        cashkarp();      // if Optimize dt is checked
    } else {
        rungekutta();    // if Optimize dt is not checked
    }

    if (t >= timeDisplay) {
        qDebug() << "Engine::runPrivate()" << t;
        emit drawGraph(this, &hpl);
        timeDisplay += timeInterval;        
    }

    if (m_sendSignals) {
        emit timeChanged(t);
        emit errorChanged(errMax);
        emit dtChanged(dt);
        emit curDenChanged(curDen);
        intervalCounter += 500;
        m_sendSignals = false;
    }

    QTimer::singleShot(0, this, &Engine::runPrivate);
}


void Engine::setup()
{
    ConstituentDb cdb;

    setB(40, 160, 320, 600);    //tohle se bude zadavat ve formulari Segments
    setErrH(1e-7);  //tohle se bude zadavat z formulare Parameters
                    //pokud bude zakliknute Optimize dt, tedy pokud bude m_optimizeDt == true


/*
    Sample &s1 = getMix().addConstituent(cdb.get(418)); // 418 Potassium
    s1.setIC(0, 0).setIC(1, 10).setIC(2, 10).setIC(3, 10);

    //Sample &s1 = getMix().addConstituent(cdb.get(216)); // 216 Ethylenediamine
    //s1.setIC(0, 0).setIC(1, 10).setIC(2, 0).setIC(3, 10);

    //Sample &s1 = getMix().addConstituent(cdb.get(441)); // 441 Sebacic acid
    //s1.setIC(0, 0.21).setIC(1, 0.21).setIC(2, 0.21).setIC(3, 0.21);

    Sample &s2 = getMix().addConstituent(cdb.get(258)); // 258 Histidine
    s2.setIC(0, 10).setIC(1, 0).setIC(2, 0).setIC(3, 0);

    Sample &s3 = getMix().addConstituent(cdb.get(399)); // 399 Phosphoric acid
    s3.setIC(0, 7.5).setIC(1, 7.5).setIC(2, 7.5).setIC(3, 7.5);

    //Sample &s4 = getMix().addConstituent(cdb.get(280)); // 280 Imidazole
    //s4.setIC(0, 0.313).setIC(1, 0.314).setIC(2, 0.313).setIC(3, 0.313);
*/


    Sample &s1 = getMix().addConstituent(cdb.get(418)); // 418 Potassium
    s1.setIC(0, 0).setIC(1, 0).setIC(2, 10).setIC(3, 10);

    Sample &s2 = getMix().addConstituent(cdb.get(85)); // 85 Acetic acid
    s2.setIC(0, 20).setIC(1, 20).setIC(2, 20).setIC(3, 20);

    Sample &s3 = getMix().addConstituent(cdb.get(523)); // 523 term
    s3.setIC(0, 10).setIC(1, 0).setIC(2, 0).setIC(3, 0);

    Sample &s4 = getMix().addConstituent(cdb.get(518)); // 518 S1
    s4.setIC(0, 0).setIC(1, 4).setIC(2, 0).setIC(3, 0);

    Sample &s5 = getMix().addConstituent(cdb.get(519)); // 519 S2
    s5.setIC(0, 0).setIC(1, 4).setIC(2, 0).setIC(3, 0);

    Sample &s6 = getMix().addConstituent(cdb.get(520)); // 520 S3
    s6.setIC(0, 0).setIC(1, 4).setIC(2, 0).setIC(3, 0);

    Sample &s7 = getMix().addConstituent(cdb.get(521)); // 521 S4
    s7.setIC(0, 0).setIC(1, 4).setIC(2, 0).setIC(3, 0);

    Sample &s8 = getMix().addConstituent(cdb.get(522)); // 522 S5
    s8.setIC(0, 0).setIC(1, 4).setIC(2, 0).setIC(3, 0);

}


#endif
