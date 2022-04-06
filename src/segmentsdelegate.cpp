/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "segmentsdelegate.h"
#include "segmentsmodel.h"
#include "constituentsdialog.h"
#include "pdebug.h"
#include <QDoubleSpinBox>
#include <QApplication>
#include <QBrush>
#include <QColor>
#include <QPainter>

SegmentsDelegate::SegmentsDelegate(QObject *parent) : QItemDelegate(parent) {
}

QWidget *SegmentsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& option, const QModelIndex &index) const {
    switch (index.row()) {
        case SegmentsModel::Ratio: {
            QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
            editor->setFrame(false);
            editor->setMinimum(0.01);
            editor->setMaximum(10000);
            editor->setDecimals(2);
            return editor;
            break;
            }
        case SegmentsModel::Concentration: {
            QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
            editor->setFrame(false);
            editor->setMinimum(0.0);
            editor->setMaximum(100000);
            editor->setDecimals(12);
            return editor;
            break;
            }
        case SegmentsModel::U3n:
        case SegmentsModel::U2n:
        case SegmentsModel::U1n: 
        case SegmentsModel::U1p:
        case SegmentsModel::U2p:
        case SegmentsModel::U3p: {
            ConstituentsDialog *dialog = qobject_cast<ConstituentsDialog *>(this->parent());
            if (dialog != nullptr && !dialog->manually()) {
                return nullptr;
                }
            QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
            editor->setFrame(false);
            editor->setMinimum(0.0);
            editor->setMaximum(1000.0);
            editor->setDecimals(16);
            return editor;
            break;
            }
        case SegmentsModel::Pk3n:
        case SegmentsModel::Pk2n:
        case SegmentsModel::Pk1n:
        case SegmentsModel::Pk1p:
        case SegmentsModel::Pk2p:
        case SegmentsModel::Pk3p: {
            ConstituentsDialog *dialog = qobject_cast<ConstituentsDialog *>(this->parent());
            if (dialog != nullptr && !dialog->manually()) {
                return nullptr;
                }
            QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
            editor->setFrame(false);
            editor->setMinimum(-6);
            editor->setMaximum(+20);
            editor->setDecimals(16);
            return editor;
            break;
            }
        }
    return QItemDelegate::createEditor(parent, option, index);
}


void SegmentsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QColor color;
    switch (index.row()) {
        case SegmentsModel::Ratio:
            QItemDelegate::paint(painter, option, index);
            return;
            break;
        case SegmentsModel::Concentration: {
            double value = index.model()->data(index).toDouble();
            QString text = QLocale().toString(value, 'g', 8);
            QStyleOptionViewItem myoption = option;
            myoption.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
            drawDisplay(painter, myoption, myoption.rect, text);
            drawFocus(painter, myoption, myoption.rect);
            // QItemDelegate::paint(painter, option, index);
            return;
            break;
            }
        case SegmentsModel::U3n:
        case SegmentsModel::U2n:
        case SegmentsModel::U1n: {
            color = QColor(222,222,255);
            break;
            }
        case SegmentsModel::U1p:
        case SegmentsModel::U2p:
        case SegmentsModel::U3p:
            color = QColor(255,222,222);
            break;
        case SegmentsModel::Pk3n:
        case SegmentsModel::Pk2n:
        case SegmentsModel::Pk1n:
            color = QColor(222,222,255);
            break;
        case SegmentsModel::Pk1p:
        case SegmentsModel::Pk2p:
        case SegmentsModel::Pk3p:
            color = QColor(255,222,222);
            break;
        };
    QBrush brush(color, Qt::SolidPattern);
    QStyleOptionViewItem myoption = option;
    myoption.backgroundBrush = brush;
    painter->save();
    painter->fillRect(myoption.rect, brush);
    painter->restore();
    QItemDelegate::paint(painter, option, index);
    return;
}

