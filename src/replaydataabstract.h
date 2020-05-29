/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _ReplayDataAbstract_H_
#define _ReplayDataAbstract_H_

#include <QVariantMap>

/**
 * @brief
 */
class ReplayDataAbstract {
  public:
    ReplayDataAbstract() {}
    virtual ~ReplayDataAbstract() {}

    virtual int size() const = 0;
    virtual QVariantMap step(int) const = 0;
};

#endif

