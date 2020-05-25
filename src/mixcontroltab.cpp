#include "mixcontroltab.h"
#include "mixcontrolmodel.h"
#include "mixcontrolswapmodel.h"
#include "mixcontroldelegate.h"
#include "ui_mixcontroltab.h"
#include "constituentsdialog.h"
#include "pdebug.h"
#include <QAction>
#include <QMessageBox>


MixControlTab::~MixControlTab()
{
    delete ui;
}


MixControlTab::MixControlTab(QWidget *parent, TabType tabType) :
    QWidget(parent),
    ui(new Ui::MixControlTab)
{
    ui->setupUi(this);
    m_tabType = tabType;

    m_model = (tabType == BasicTab) 
            ? qobject_cast<MixControlModelAbstract *>(new MixControlModel(this))
            : qobject_cast<MixControlModelAbstract *>(new MixControlSwapModel(this));
    ui->f_view->setModel(m_model);
    ui->f_view->setItemDelegate(new MixControlDelegate(this));
    ui->f_view->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(m_model, &MixControlModel::visibilityChanged, this, &MixControlTab::visibilityChanged);

    resizeColumns();

    QAction *action;
    action = new QAction("Add", this);
    ui->f_add->setDefaultAction(action);
    connect(action, &QAction::triggered, this, &MixControlTab::addComponent);
    ui->f_view->addAction(action);

    action = new QAction("Remove", this);
    action->setEnabled(false);
    ui->f_remove->setDefaultAction(action);
    connect(action, &QAction::triggered, this, &MixControlTab::removeComponent);
    connect(ui->f_view, &MyView::currentRowChanged, [this,action]() {
        action->setEnabled(ui->f_view->currentIndex().isValid());
        });
    ui->f_view->addAction(action);

    action = new QAction("Edit", this);
    action->setEnabled(false);
    ui->f_edit->setDefaultAction(action);
    connect(action, &QAction::triggered, this, QOverload<>::of(&MixControlTab::editComponent));
    connect(ui->f_view, &MyView::currentRowChanged, [this,action]() {
        action->setEnabled(ui->f_view->currentIndex().isValid());
        });
    ui->f_view->addAction(action);

    action = new QAction("Remove All", this);
    ui->f_removeAll->setDefaultAction(action);
    connect(action, &QAction::triggered, this, &MixControlTab::removeAll);

    action = new QAction("Swap", this);
    ui->f_swap->setDefaultAction(action);
    connect(action, &QAction::triggered, this, &MixControlTab::swapPressed);

    ui->f_segment->setVisible(tabType != BasicTab);
    ui->f_swap->setVisible(tabType != BasicTab);

    connect(ui->f_view, &QAbstractItemView::doubleClicked, this, QOverload<const QModelIndex&>::of(&MixControlTab::editComponent));
    connect(ui->f_view, &QAbstractItemView::clicked, m_model, &MixControlModel::toggleVisible);
    connect(ui->f_segment, &SwapSegmentWidget::segmentsChanged, this, &MixControlTab::recalculateModel);
}


void MixControlTab::recalculateModel() {
    m_model->recalculate(ui->f_segment->ratios());
}


void MixControlTab::setRatios(const QList<int>& ratios) {
    ui->f_segment->setRatios(ratios);
}


void MixControlTab::swapPressed() {
    ui->f_swap->setEnabled(false);
    emit swap(m_model->constituents());
}


bool MixControlTab::removable() const {
    return ui->f_swap->isEnabled();
}


void MixControlTab::resizeColumns() {
    for (int i=0; i<MixControlModel::LastCol; i++) {
        ui->f_view->resizeColumnToContents(i);
        }
}


void MixControlTab::addComponent() {
    ConstituentsDialog dialog;
    if (m_tabType == SwapTab) { dialog.set3FixedSegments(ui->f_segment->ratios()); }
    if (QDialog::Accepted == dialog.exec()) {
        SegmentedConstituent c = dialog.constituent();
        QModelIndex index = m_model->add(c);
        Q_UNUSED(index);
        resizeColumns();
        }
}


void MixControlTab::editComponent() {
    editComponent(ui->f_view->currentIndex());
}


void MixControlTab::editComponent(const QModelIndex& index) {
    int row = index.row();
    ConstituentsDialog dialog;
    dialog.setConstituent(m_model->constituent(row));
    if (m_tabType == SwapTab) { dialog.set3FixedSegments(ui->f_segment->ratios()); }
    if (QDialog::Accepted == dialog.exec()) {
        SegmentedConstituent c = dialog.constituent();
        m_model->setConstituent(c, row);
        resizeColumns();
        }
}


void MixControlTab::removeComponent() {
    QModelIndex index = ui->f_view->currentIndex();
    m_model->removeRows(index.row(), 1);
}


void MixControlTab::removeAll() {
    int rc= QMessageBox::question(this,
            tr("I have a question"),
            tr("Do you really want to remove all constituents?"),
            QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);
    if (rc == QMessageBox::Yes) {
        m_model->removeRows(0, m_model->rowCount());
        }

}


void MixControlTab::add(const QList<SegmentedConstituent>& list)  {
    m_model->add(list);
}


void MixControlTab::init() {
    m_model->enableConstituents();
    ui->f_swap->setEnabled(true);
}


void MixControlTab::disableConstituent(int internalId) {
    m_model->disableConstituent(internalId);
}


void MixControlTab::hideConstituent(int internalId) {
    m_model->hide(internalId);
}


