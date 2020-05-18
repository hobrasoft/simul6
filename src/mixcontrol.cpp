#include "mixcontrol.h"
#include "mixcontroldelegate.h"
#include "ui_mixcontrol.h"
#include "constituentsdialog.h"
#include "pdebug.h"
#include <QAction>
#include <QToolButton>
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

    m_basicTab = new MixControlTab(this, MixControlTab::BasicTab);
    ui->f_tab->addTab(m_basicTab, tr("Basic"));

    QToolButton *button = new QToolButton(this);
    ui->f_tab->setCornerWidget(button);

    m_addTab = new QAction(QIcon("://icons/copy.svg"), tr("Add empty mix"));
    button->setDefaultAction(m_addTab);
    connect(m_addTab, &QAction::triggered, [this]() {
        MixControlTab *newTab = new MixControlTab(this);
        ui->f_tab->addTab(newTab, tr("Swap"));
        ui->f_tab->setCurrentIndex( ui->f_tab->count()-1 );
        });

    m_cloneTab = new QAction(tr("Clone current mix"));
    button->addAction(m_cloneTab);
    connect(m_cloneTab, &QAction::triggered, [this]() {
        MixControlTab *newTab = new MixControlTab(this);
        ui->f_tab->addTab(newTab, tr("Swap"));
        ui->f_tab->setCurrentIndex( ui->f_tab->count()-1 );
        });

    m_removeTab = new QAction(tr("Remove current mix"));
    button->addAction(m_removeTab);
    connect(m_removeTab, &QAction::triggered, this, &MixControl::removeCurrentTab);

    connect(ui->f_tab, &QTabWidget::currentChanged, this, &MixControl::currentTabChanged);

    currentTabChanged(0);
}


void MixControl::currentTabChanged(int index) {
    if (index == 0) { m_removeTab->setEnabled(false); return; }
    m_removeTab->setEnabled(true);
}


void MixControl::removeCurrentTab() {
    ui->f_tab->removeTab(ui->f_tab->currentIndex());
}


const MixControlModelAbstract *MixControl::model() const {
    return m_basicTab->model();
}

void MixControl::resizeColumns() {
    m_basicTab->resizeColumns();
}

void MixControl::hideConstituent(int internalId) {
    m_basicTab->hideConstituent(internalId);
}

