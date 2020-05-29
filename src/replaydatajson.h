/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _ReplayDataJson_H_
#define _ReplayDataJson_H_

#include "replaydataabstract.h" 
#include <QVariantList>

/**
 * @brief
 */
class ReplayDataJson : public ReplayDataAbstract {
  public:
    ReplayDataJson(const QVariantList& data);

    int size() const Q_DECL_OVERRIDE;
    QVariantMap step(int) const Q_DECL_OVERRIDE;

  private:
    QVariantList m_data;
};

#endif

