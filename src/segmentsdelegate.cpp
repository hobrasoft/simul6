/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "segmentsdelegate.h"
#include "segmentsmodel.h"
#include "pdebug.h"
#include <QDoubleSpinBox>
#include <QApplication>

SegmentsDelegate::SegmentsDelegate(QObject *parent) : QStyledItemDelegate(parent) {
}

QWidget *SegmentsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& option, const QModelIndex &index) const {
    if (index.row() != SegmentsModel::Concentration) {
        return QStyledItemDelegate::createEditor(parent, option, index);
        }

    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setFrame(false);
    editor->setMinimum(0.0);
    editor->setMaximum(1000);
    editor->setDecimals(4);
    return editor;
}

