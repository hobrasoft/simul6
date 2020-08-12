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
    connect(ui->f_view->selectionModel(), &QItemSelectionModel::currentChanged, this, &MixControlTab::currentChanged);
    connect(ui->f_view->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MixControlTab::selectionChanged);

    resizeColumns();

    m_actionAdd = new QAction("Add", this);
    ui->f_add->setDefaultAction(m_actionAdd);
    connect(m_actionAdd, &QAction::triggered, this, &MixControlTab::addComponent);
    ui->f_view->addAction(m_actionAdd);

    m_actionRemove= new QAction("Remove", this);
    m_actionRemove->setEnabled(false);
    ui->f_remove->setDefaultAction(m_actionRemove);
    connect(m_actionRemove, &QAction::triggered, this, &MixControlTab::removeComponent);
    ui->f_view->addAction(m_actionRemove);

    m_actionEdit = new QAction("Edit", this);
    m_actionEdit->setEnabled(false);
    ui->f_edit->setDefaultAction(m_actionEdit);
    connect(m_actionEdit, &QAction::triggered, this, QOverload<>::of(&MixControlTab::editComponent));
    ui->f_view->addAction(m_actionEdit);

    m_actionToggleVisibility = new QAction("Toggle visibility", this);
    m_actionToggleVisibility->setEnabled(false);
    ui->f_toggle->setDefaultAction(m_actionToggleVisibility);
    connect(m_actionToggleVisibility, &QAction::triggered, this, &MixControlTab::toggleVisibility);
    ui->f_view->addAction(m_actionToggleVisibility);

    m_actionRemoveAll = new QAction("Remove All", this);
    ui->f_removeAll->setDefaultAction(m_actionRemoveAll);
    connect(m_actionRemoveAll, &QAction::triggered, this, &MixControlTab::removeAll);

    m_actionSwap = new QAction("Swap", this);
    ui->f_swap->setDefaultAction(m_actionSwap);
    m_actionSwap->setEnabled(false);
    connect(m_actionSwap, &QAction::triggered, this, &MixControlTab::swapPressed);

    ui->f_segment->setVisible(tabType != BasicTab);
    ui->f_swap->setVisible(tabType != BasicTab);

    connect(ui->f_view, &QAbstractItemView::doubleClicked, this, QOverload<const QModelIndex&>::of(&MixControlTab::editComponent));
    connect(ui->f_view, &QAbstractItemView::clicked, m_model, &MixControlModel::toggleVisible);
    connect(ui->f_segment, &SwapSegmentWidget::segmentsChanged, this, &MixControlTab::recalculateModel);
}


void MixControlTab::currentChanged(const QModelIndex& current, const QModelIndex&) {
    ui->f_view->scrollTo(current);
    enableActions();
}


void MixControlTab::selectionChanged(const QItemSelection&, const QItemSelection&) {
    enableActions();
}


void MixControlTab::enableActions() {
    bool hasSelection = ui->f_view->selectionModel()->hasSelection();
    int number = ui->f_view->selectionModel()->selectedRows().size();
    m_actionRemove->setEnabled(hasSelection);
    m_actionToggleVisibility->setEnabled(hasSelection);
    m_actionEdit->setEnabled(number == 1);
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
    if (m_model->rowCount() <= 0) { return true; }
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
        m_actionSwap->setEnabled(m_model->rowCount() != 0);
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
    QModelIndexList selectedRows = ui->f_view->selectionModel()->selectedRows();
    QMap<int, int> rowsToRemove;
    for (int i=0; i<selectedRows.size(); i++) {
        rowsToRemove[selectedRows[i].row()] = 0;
        }
    QMapIterator<int, int> iterator(rowsToRemove);
    iterator.toBack();
    while (iterator.hasPrevious()) {
        iterator.previous();
        int row = iterator.key();
        m_model->removeRows(row, 1);
        }

    m_actionSwap->setEnabled(m_model->rowCount() != 0);
}


void MixControlTab::toggleVisibility() {
    QModelIndexList selectedRows = ui->f_view->selectionModel()->selectedRows();
    for (int i=0; i<selectedRows.size(); i++) {
        m_model->toggleVisible(selectedRows[i]);
        }
}


void MixControlTab::removeAll() {
    int rc= QMessageBox::question(this,
            tr("I have a question"),
            tr("Do you really want to remove all constituents?"),
            QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);
    if (rc == QMessageBox::Yes) {
        m_model->removeRows(0, m_model->rowCount());
        }
    m_actionSwap->setEnabled(m_model->rowCount() != 0);
}


void MixControlTab::add(const QList<SegmentedConstituent>& list)  {
    m_model->add(list);
    m_actionSwap->setEnabled(m_model->rowCount() != 0);
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


