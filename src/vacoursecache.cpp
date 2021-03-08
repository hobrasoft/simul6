/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "vacoursecache.h"
#include "Engine.h"
#include <math.h>
#include "pdebug.h"


VACourseCache::VACourseCache() : QObject() {
}


void VACourseCache::clear() {
    lock();
    m_A.clear();
    m_V.clear();
    unlock();
}


void VACourseCache::appendData(const Engine* pEngine) {
    pEngine->lock();
    lock();
    if (pEngine->getCapLen() <= 0) { 
        pEngine->unlock();
        return; 
        }

    double time = pEngine->getTime();
    m_A << QPointF(time, pEngine->curDen * 1e6);
    m_V << QPointF(time, pEngine->voltage);

    pEngine->unlock();
    unlock();
}

