/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "idgenerator.h"

int IdGenerator::m_lastId = -2;


int IdGenerator::nextId() {
    m_lastId -= 2;
    return m_lastId;
}


int IdGenerator::lastId() {
    return m_lastId;
}


void IdGenerator::reset() {
    m_lastId = -2;
}



