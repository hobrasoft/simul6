#include "mixcontrol.h"
#include "ui_mixcontrol.h"
#include "constituentsdialog.h"
#include <QAction>
#include <QMessageBox>


MixControl::~MixControl()
{
    delete ui;
}


MixControl::MixControl(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::MixControl)
{
    ui->setupUi(this);

    m_modelReal = new MixControlModel(this);
    m_model = new QSortFilterProxyModel(this);
    m_model->setSourceModel(m_modelReal);
    ui->f_view->setModel(m_model);

    resizeColumns();

    QAction *action;
    action = new QAction("Add", this);
    ui->f_add->setDefaultAction(action);
    connect(action, &QAction::triggered, this, &MixControl::addComponent);
    ui->f_view->addAction(action);

    action = new QAction("Remove", this);
    action->setEnabled(false);
    ui->f_remove->setDefaultAction(action);
    connect(action, &QAction::triggered, this, &MixControl::removeComponent);
    connect(ui->f_view, &MyView::currentRowChanged, [this,action]() {
        action->setEnabled(ui->f_view->currentIndex().isValid());
        });
    ui->f_view->addAction(action);

    action = new QAction("Edit", this);
    action->setEnabled(false);
    ui->f_edit->setDefaultAction(action);
    connect(action, &QAction::triggered, this, QOverload<>::of(&MixControl::editComponent));
    connect(ui->f_view, &MyView::currentRowChanged, [this,action]() {
        action->setEnabled(ui->f_view->currentIndex().isValid());
        });
    ui->f_view->addAction(action);

    action = new QAction("Remove All", this);
    ui->f_removeAll->setDefaultAction(action);
    connect(action, &QAction::triggered, this, &MixControl::removeAll);
    ui->f_view->addAction(action);

    connect(ui->f_view, &QAbstractItemView::doubleClicked, this, QOverload<const QModelIndex&>::of(&MixControl::editComponent));
}


void MixControl::resizeColumns() {
    for (int i=0; i<MixControlModel::LastCol; i++) {
        ui->f_view->resizeColumnToContents(i);
        }
}


void MixControl::addComponent() {
    ConstituentsDialog dialog;
    if (QDialog::Accepted == dialog.exec()) {
        Constituent c = dialog.constituent();
        Segments s = dialog.segments();
        QModelIndex index = m_modelReal->add(c, s);
        Q_UNUSED(index);
        // QItemSelectionModel sm;
        // sm.select(index, QItemSelectionModel::Rows);
        // ui->f_view->setSelectionModel(&sm);
        resizeColumns();
        }
}


void MixControl::editComponent() {
    editComponent(ui->f_view->currentIndex());
}


void MixControl::editComponent(const QModelIndex& index) {
    int row = m_model->mapToSource(index).row();
    ConstituentsDialog dialog;
    dialog.setConstituent(m_modelReal->constituent(row));
    dialog.setSegments(m_modelReal->segments(row));
    if (QDialog::Accepted == dialog.exec()) {
        Constituent c = dialog.constituent();
        Segments s = dialog.segments();
        m_modelReal->setConstituentAndSegments(c, s, row);
        resizeColumns();
        }
}


void MixControl::removeComponent() {
    QModelIndex index = ui->f_view->currentIndex();
    m_model->removeRows(index.row(), 1);
}


void MixControl::removeAll() {
    int rc= QMessageBox::question(this,
            tr("I have a question"),
            tr("Do you really want to remove all constituents?"),
            QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);
    if (rc == QMessageBox::Yes) {
        m_model->removeRows(0, m_model->rowCount());
        }

}


