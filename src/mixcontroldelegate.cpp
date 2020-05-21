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

    Qt::ItemFlags flags = index.model()->flags(index);
    bool enabled = ((flags & Qt::ItemIsEnabled) != Qt::ItemIsEnabled);
    QStyleOptionButton style;
    style.state = (enabled) ? QStyle::State_Enabled : QStyle::State_None;
    style.state |= (index.model()->data(index).toBool()) ? QStyle::State_On : QStyle::State_Off;
    style.rect = option.rect;

    QApplication::style()->drawControl(QStyle::CE_CheckBox, &style, painter);
}

