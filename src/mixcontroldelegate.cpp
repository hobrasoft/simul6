/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "mixcontroldelegate.h"
#include "mixcontrolmodel.h"
#include "pdebug.h"
#include <QStyleOptionButton>
#include <QApplication>

MixControlDelegate::MixControlDelegate(QObject *parent) : QStyledItemDelegate(parent) {
}

void MixControlDelegate::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    if (index.column() != MixControlModel::Visible) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
        }

    QStyleOptionButton style;
    style.state = QStyle::State_Enabled;
    style.state |= (index.model()->data(index).toBool()) ? QStyle::State_On : QStyle::State_Off;
    style.rect = option.rect;
    QApplication::style()->drawControl(QStyle::CE_CheckBox, &style, painter);

}

