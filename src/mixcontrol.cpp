#include "mixcontrol.h"
#include "mixcontroldelegate.h"
#include "ui_mixcontrol.h"
#include "constituentsdialog.h"
#include "pdebug.h"
#include <QAction>
#include <QMessageBox>


MixControl::~MixControl()
{
    delete ui;
}


MixControl::MixControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MixControl)
{
    ui->setupUi(this);

    m_model = new MixControlModel(this);
    ui->f_view->setModel(m_model);
    ui->f_view->setItemDelegate(new MixControlDelegate(this));
    connect(m_model, &MixControlModel::visibilityChanged, this, &MixControl::visibilityChanged);

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
    connect(ui->f_view, &QAbstractItemView::clicked, m_model, &MixControlModel::toggleVisible);
}


void MixControl::resizeColumns() {
    for (int i=0; i<MixControlModel::LastCol; i++) {
        ui->f_view->resizeColumnToContents(i);
        }
}


void MixControl::addComponent() {
    ConstituentsDialog dialog;
    if (QDialog::Accepted == dialog.exec()) {
        SegmentedConstituent c = dialog.constituent();
        QModelIndex index = m_model->add(c);
        Q_UNUSED(index);
        resizeColumns();
        }
}


void MixControl::editComponent() {
    editComponent(ui->f_view->currentIndex());
}


void MixControl::editComponent(const QModelIndex& index) {
    int row = index.row();
    ConstituentsDialog dialog;
    dialog.setConstituent(m_model->constituent(row));
    if (QDialog::Accepted == dialog.exec()) {
        SegmentedConstituent c = dialog.constituent();
        m_model->setConstituent(c, row);
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


void MixControl::hideConstituent(int internalId) {
    m_model->hide(internalId);
}


