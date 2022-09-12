/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _LinesDelegate_H_
#define _LinesDelegate_H_

#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>

/**
 * @brief
 */
class LinesDelegate : public QItemDelegate {
    Q_OBJECT
  public:
    LinesDelegate(QObject *parent);


  private slots:
    void editorFinished(int result);

  private:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem& option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif

