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
const double Engine::uHpl = 363e-9;
const double Engine::uOHmin = 205e-9;
const double Engine::difHpl = Engine::uHpl * Engine::r * Engine::te/ Engine::farc;
const double Engine::difOHmin = Engine::uOHmin * Engine::r * Engine::te / Engine::farc;
const double Engine::pKw = 14;
const double Engine::kw = exp(-Engine::pKw * log(10));
const unsigned int Engine::maxCharge = 3;

using namespace std;

Engine::Engine(unsigned int pAreas, int pNp) :
    np(pNp),
    numb(0),
    t(0),
    concUp(0),
    concDown(0),
    critG(0),
    c0(1000),
    mix(pAreas, pNp),
    m_initialized(false),
    m_running(false),
    m_sendSignals(false),
    m_iterations(0)
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

/*void Engine::addMix(const Mix *m)
{

}*/

Mix &Engine::getMix()
{
    return mix;
}


size_t Engine::getNm()
{
    return mix.size();
}

size_t Engine::getConstituentCount()
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
    difPot.resize(np + 1, 0);
}

void Engine::init()
{
    qDebug() << "Engine::init()";
    m_initialized = true;
    m_iterations = 0;
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
    emit drawGraph(&mix, &hpl);
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
            for (unsigned int k = 0; k < getConstituentCount(); k++) {
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
        for (unsigned int k = 0; k < getConstituentCount(); k++) {
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
//#pragma omp parallel for schedule(static)
//        for (int i = 0; i <= np; i++) double kuk = 1.0;

//    gCalc();
    der();

#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        for (auto &s : mix.getSamples()) {
            s.setQ2(i, s.getD(0, i) * dt);
            s.setA(0, i, s.getV(i) + beta31 * s.getQ1(i) + beta32 * s.getQ2(i));
        }
    }
//    gCalc();
    der();

#pragma omp parallel for schedule(static)
    for (int i = 0; i <= np; i++) {
        for (auto &s : mix.getSamples()) {
            s.setQ3(i, s.getD(0, i) * dt);
            s.setA(0, i, s.getV(i) + beta41 * s.getQ1(i) - beta42 * s.getQ2(i) + beta43 * s.getQ3(i));
        }
    }
//    gCalc();
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
    m_running = true;
    QTimer::singleShot(0, this, &Engine::runPrivate);
}

void Engine::runPrivate() {
//  qDebug() << "Engine::runPrivate()" << m_iterations;

    if (!m_running || m_iterations++ > 50000) {
        emit drawGraph(&mix, &hpl);
        emit timeChanged(t);
        emit finished();
        return;
    }

    rungekutta();
    if (m_iterations % numb == 0) {
        qDebug() << "Engine::runPrivate()" << m_iterations;
        emit drawGraph(&mix, &hpl);
    }

    if (m_sendSignals) {
        emit timeChanged(t);
        m_sendSignals = false;
    }

    QTimer::singleShot(0, this, &Engine::runPrivate);
}

void Engine::show()
{
    cout << "Showing engine parameters:" << endl;
    cout << "- dx = " << dx << endl;
    cout << "- difs:" << endl;
    for (unsigned int i = 0; i < getNm(); i++) {
        cout << "  - dif[" << i << "] = " << dif[i] << endl;
    }
    /*
    cout << "- a:" << endl;
    for (int i = 0; i <= getNp(); i++) {
    cout << "  - a[" << i << "][0] = " << a[i][0] << endl;
    }*/

    //console.drawSample(a, 0);
}

void Engine::setup()
{
    ConstituentDb cdb;

    setB(20, 200, 400, 600);
    setCapLen(CAPLEN);
    setCurDen(-30);
    setDt(0.01);
    setConcUp(20);
    setConcDown(0);

    setNumb(5000);
    setCritG(1e-6);
    setC0(1000);

    Sample &s1 = getMix().addConstituent(cdb.get(418)); // 418 Potassium
    s1.setIC(0, 0).setIC(1, 10).setIC(2, 10).setIC(3, 10);

    //Sample &s1 = getMix().addConstituent(cdb.get(216)); // 216 Ethylenediamine
    //s1.setIC(0, 0).setIC(1, 10).setIC(2, 0).setIC(3, 10);

    //Sample &s1 = getMix().addConstituent(cdb.get(441)); // 441 Sebacic acid
    //s1.setIC(0, 0.21).setIC(1, 0.21).setIC(2, 0.21).setIC(3, 0.21);



    Sample &s2 = getMix().addConstituent(cdb.get(258)); // 258 Histidine
    s2.setIC(0, 10).setIC(1, 0).setIC(2, 0).setIC(3, 0);

    //Sample &s2 = getMix().addConstituent(cdb.get(280)); // 280 Imidazole
    //s2.setIC(0, 0.313).setIC(1, 0.314).setIC(2, 0.313).setIC(3, 0.313);



    Sample &s3 = getMix().addConstituent(cdb.get(399)); // 399 Phosphoric acid
    s3.setIC(0, 7.5).setIC(1, 7.5).setIC(2, 7.5).setIC(3, 7.5);


}


#endif
