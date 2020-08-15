#include "mixcontrol.h"
#include "mixcontroldelegate.h"
#include "ui_mixcontrol.h"
#include "constituentsdialog.h"
#include "msettings.h"
#include "pdebug.h"
#include <QAction>
#include <QToolButton>
#include <QTimer>


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
    connect(m_basicTab, &MixControlTab::visibilityChanged, this, &MixControl::visibilityChanged);

    QToolButton *button = new QToolButton(this);
    button->setContextMenuPolicy(Qt::ActionsContextMenu);
    button->setVisible(false);

    m_addTab = new QAction(tr("Add swap content"));
    m_addTab->setToolTip(tr("Add swap content to be swapped in part of capilary."));
    button->setDefaultAction(m_addTab);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    // button->setStyleSheet("background-color:#606060;color:white");
    button->setStyleSheet("background-color:darkgray");
    connect(m_addTab, &QAction::triggered, [this]() {
        createNewSwap();
        });

    m_cloneTab = new QAction(tr("Clone current swap content"));
    button->addAction(m_cloneTab);
    connect(m_cloneTab, &QAction::triggered, [this]() {
        MixControlTab *currentTab = qobject_cast<MixControlTab *>(ui->f_tab->currentWidget());
        QList<SegmentedConstituent> data = currentTab->model()->constituents();
        createNewSwap(data);
        });

    m_removeTab = new QAction(tr("Remove current swap content"));
    button->addAction(m_removeTab);
    connect(m_removeTab, &QAction::triggered, this, &MixControl::removeCurrentTab);


    m_viewSwap = new QAction(tr("Swapping"));
    m_viewSwap->setCheckable(true);
    m_viewSwap->setChecked(false);
    connect(m_viewSwap, &QAction::triggered, [this,button]() {
        bool visible = m_viewSwap->isChecked();
        QWidget *w = (visible) ? button : nullptr; 
        ui->f_tab->setCornerWidget(w, Qt::TopLeftCorner);
        button->setVisible(visible);
        writeSettings();
        });

    connect(ui->f_tab, &QTabWidget::currentChanged, this, &MixControl::currentTabChanged);

    currentTabChanged(0);
    readSettings();

    QTimer::singleShot(1, m_viewSwap, &QAction::trigger);
}


void MixControl::readSettings() {
    bool visible = MSETTINGS->guiShowSwapButton();
    m_viewSwap->setChecked(!visible); // Should be negated!
}


void MixControl::writeSettings() {
    MSETTINGS->setGuiShowSwapButton(m_viewSwap->isChecked());
}


MixControlTab *MixControl::createNewSwap(const QList<SegmentedConstituent>& data) {
    MixControlTab *newTab = new MixControlTab(this);
    ui->f_tab->addTab(newTab, tr("Swap content"));
    ui->f_tab->setCurrentIndex( ui->f_tab->count()-1 );
    connect(newTab, &MixControlTab::swap, this, &MixControl::swap);
    connect(newTab, &MixControlTab::swap, [this]() { currentTabChanged(-1); });
    connect(newTab, &MixControlTab::visibilityChanged, this, &MixControl::visibilityChanged);
    newTab->add(data); 
    return newTab;
}


void MixControl::currentTabChanged(int index) {
    if (index == 0) { m_removeTab->setEnabled(false); return; }
    MixControlTab *x = qobject_cast<MixControlTab *>(ui->f_tab->currentWidget());
    m_removeTab->setEnabled(x->removable());
}


void MixControl::removeCurrentTab() {
    ui->f_tab->removeTab(ui->f_tab->currentIndex());
}


void MixControl::setMixJson(const QVariantList& json) {
    const_cast<MixControlModelAbstract *>(m_basicTab->model())->setJson(json);
}


QVariantList MixControl::mixJson() const {
    return m_basicTab->model()->json();
}

QVariantList MixControl::swapsJson() const {
    QVariantList list;
    for (int i=1; i<ui->f_tab->count(); i++) {
        MixControlTab *tab = qobject_cast<MixControlTab *>(ui->f_tab->widget(i));
        if (tab == nullptr) { continue; }
        QVariantMap mix;
        mix["mix"] = tab->model()->json();
        list << mix;
        }
    return list;
}


void MixControl::setSwapsJson(const QVariantList& json) {
    while (ui->f_tab->count() != 1) {
        ui->f_tab->removeTab(1);
        }

    for (int i=0; i<json.size(); i++) {
        const QVariantMap& swap = json[i].toMap();
        const QVariantList& mix = swap["mix"].toList();
        QList<SegmentedConstituent> list;
        QList<int> ratios;
        for (int c=0; c<mix.size(); c++) {
            SegmentedConstituent constituent = SegmentedConstituent(mix[c].toMap()["constituent"].toMap());
            // PDEBUG << constituent << mix[c].toMap();
            list << constituent;
            if (c==0) {
                for (int s=0; s<constituent.segments.size(); s++) {
                    ratios << constituent.segments[s].ratio;
                    }
                }
            }
        createNewSwap(list)->setRatios(ratios);
        }
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


int MixControl::mixSize() const {
    int size = 0;
    for (int i=0; i<ui->f_tab->count(); i++) {
        MixControlTab *tab = qobject_cast<MixControlTab *>(ui->f_tab->widget(i));
        if (tab == nullptr) { continue; }
        size += tab->model()->rowCount();
        }
    return size;
}


void MixControl::disableConstituent(int internalId) {
    for (int i=0; i<ui->f_tab->count(); i++) {
        MixControlTab *tab = qobject_cast<MixControlTab *>(ui->f_tab->widget(i));
        if (tab == nullptr) { continue; }
        tab->disableConstituent(internalId);
        }
}


void MixControl::init() {
    m_removeTab->setEnabled(true);
    for (int i=0; i<ui->f_tab->count(); i++) {
        MixControlTab *tab = qobject_cast<MixControlTab *>(ui->f_tab->widget(i));
        if (tab == nullptr) { continue; }
        tab->init();
        }
}

