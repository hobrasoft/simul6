/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _SegmentsDelegate_H_
#define _SegmentsDelegate_H_

#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>

/**
 * @brief
 */
class SegmentsDelegate : public QStyledItemDelegate {
    Q_OBJECT
  public:
    SegmentsDelegate(QObject *parent);

  private:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem& option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif

