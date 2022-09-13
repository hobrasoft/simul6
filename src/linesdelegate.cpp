/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "linesdelegate.h"
#include "linesmodel.h"
#include "pdebug.h"
#include "simul6.h"
#include <QDoubleSpinBox>
#include <QBrush>
#include <QColor>
#include <QColorDialog>
#include <QPainter>

LinesDelegate::LinesDelegate(QObject *parent) : QItemDelegate(parent) {
}

QWidget *LinesDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& option, const QModelIndex &index) const {
    switch (index.column()) {
        case LinesModel::Position: {
            QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
            editor->setFrame(false);
            editor->setMinimum(0.0);
            editor->setMaximum(Simul6::instance()->getCaplen() * 1e3);
            editor->setDecimals(3);
            return editor;
            }
        case LinesModel::Color: {
            QWidget *e = new QWidget(parent);
            QDialog *d = new QColorDialog(e);
            connect(d, &QDialog::finished, this, &LinesDelegate::editorFinished);
            d->setObjectName("LinesColorDialog");
            d->setModal(true);
            d->show();
            return e;
            }
        }
    return QItemDelegate::createEditor(parent, option, index);
}


void LinesDelegate::editorFinished(int result) {
    QWidget *editor = qobject_cast<QWidget *>(sender());
    QWidget *parent = qobject_cast<QWidget *>(editor->parent());
    emit commitData(parent);
}


void LinesDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    switch (index.column()) {
        case LinesModel::Position: {
            PDEBUG;
            qobject_cast<QDoubleSpinBox *>(editor)->setValue(index.model()->data(index).toDouble());
            return;
            }
        case LinesModel::Color: {
            QVariant value = index.model()->data(index);
            QColor color = value.value<QColor>();
            QColorDialog *d = editor->findChild<QColorDialog *>("LinesColorDialog");
            d->setCurrentColor(color);
            return;
            }
        }
    QItemDelegate::setEditorData(editor, index);
}


void LinesDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    switch (index.column()) {
        case LinesModel::Position: {
            model->setData(index, qobject_cast<QDoubleSpinBox *>(editor)->value());
            return;
            }
        case LinesModel::Color: {
            QColorDialog *d = editor->findChild<QColorDialog *>("LinesColorDialog");
            QColor color = d->currentColor();
            model->setData(index, color);
            return;
            }
        }
    QItemDelegate::setModelData(editor, model, index);
}


void LinesDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    switch (index.column()) {
        case LinesModel::Color: {
            QVariant value = index.model()->data(index);
            QColor bgcolor = value.value<QColor>();
            QBrush brush(bgcolor, Qt::SolidPattern);
            QStyleOptionViewItem myoption = option;
            myoption.backgroundBrush = brush;
            painter->save();
            painter->fillRect(myoption.rect, brush);
            painter->restore();
            return;
            }
        case LinesModel::Position:
            break;
        };
    QItemDelegate::paint(painter, option, index);
    return;
}

