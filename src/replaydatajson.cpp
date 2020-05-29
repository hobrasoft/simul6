/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "replaydatajson.h"


ReplayDataJson::ReplayDataJson(const QVariantList& data) : ReplayDataAbstract() {
    m_data = data;
}


int ReplayDataJson::size() const {
    return m_data.size();
}


QVariantMap ReplayDataJson::step(int x) const {
    return m_data[x].toMap();
}

