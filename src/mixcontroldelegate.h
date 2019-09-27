/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _MixControlDelegate_H_
#define _MixControlDelegate_H_

#include <QStyledItemDelegate>

/**
 * @brief
 */
class MixControlDelegate : public QStyledItemDelegate {
    Q_OBJECT
  public:
    MixControlDelegate(QObject *parent);

  private:
    void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;

};

#endif

