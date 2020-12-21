/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "detectorcache.h"
#include "Engine.h"
#include <math.h>
#include "pdebug.h"


DetectorCache::DetectorCache() : QObject() {
    m_detectorPosition = 0;
}


void DetectorCache::clear() {
    lock();
    m_data.clear();
    unlock();
}


void DetectorCache::appendData(const Engine* pEngine) {
    pEngine->lock();
    if (pEngine->getCapLen() <= 0) { 
        pEngine->unlock();
        return; 
        }

    double time = pEngine->getTime();
    int detector_position = pEngine->getNp() * m_detectorPosition / (pEngine->getCapLen() * 1000.0);

    QList<QPointF> cacheItem;
    int id = 0;

    auto hpl = pEngine->getHpl();
    double pH = (hpl[detector_position] > 0) ? (-log(hpl[detector_position]) / log(10)) : 0;
    cacheItem << QPointF(time, pH);
    id += 1;

    auto kapal = pEngine->getKapa();
    double kapa = kapal[detector_position] * 100.0;
    cacheItem << QPointF(time, kapa);
    id += 1;

    const Mix& mix = pEngine->getMix();
    for (auto &sample : mix.getSamples()) {
        cacheItem << QPointF(time, sample.getA(0, detector_position));
        id += 1;
        }

    pEngine->unlock();

    lock();
    m_data << cacheItem;
    unlock();
}

