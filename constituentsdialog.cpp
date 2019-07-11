#include "constituentsdialog.h"
#include "constituentsmodel.h"
#include "ui_constituentsdialog.h"
#include "parametersmodel.h"
#include <QSortFilterProxyModel>
#include <QRegExp>

ConstituentsDialog::ConstituentsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConstituentsDialog)
{
    ui->setupUi(this);
    ConstituentsModel *databaseModel = new ConstituentsModel(this);
    QSortFilterProxyModel *databaseProxyModel = new QSortFilterProxyModel(this);
    databaseProxyModel->setSourceModel(databaseModel);
    ui->f_databaseView->setModel(databaseProxyModel);
    connect(databaseModel, &ConstituentsModel::loaded, this, &ConstituentsDialog::modelLoaded);
    connect(ui->f_manuallyGroupBox, &QGroupBox::toggled, this, &ConstituentsDialog::enableGroupBoxes);
    enableGroupBoxes();
    databaseProxyModel->setFilterKeyColumn(ConstituentsModel::Name);
    databaseProxyModel->sort(ConstituentsModel::Name);
    connect(ui->f_search, &QLineEdit::textEdited, [=](const QString& text) {
        databaseProxyModel->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive));
    });

    m_segmentsModel = new SegmentsModel(this);
    m_segmentsModel->setSegmentsNumber(ui->f_segmentsNumber->value());
    ui->f_segmentsView->setModel(m_segmentsModel);
    connect(ui->f_segmentsNumber, SIGNAL(valueChanged(int)), m_segmentsModel, SLOT(setSegmentsNumber(int)));

    m_parametersModel = new ParametersModel(this);
    ui->f_parametersView->setModel(m_parametersModel);

}

ConstituentsDialog::~ConstituentsDialog()
{
    delete ui;
}

void ConstituentsDialog::enableGroupBoxes() {
    bool manually = ui->f_manuallyGroupBox->isChecked();
    ui->f_segmentsGroupBox->setEnabled(true);
    ui->f_databaseGroupBox->setEnabled(!manually);
    ui->f_parametersGroupBox->setEnabled(manually);
}

void ConstituentsDialog::modelLoaded() {
    ui->f_databaseView->resizeColumnToContents(ConstituentsModel::Id);
    ui->f_databaseView->resizeColumnToContents(ConstituentsModel::Name);
    ui->f_databaseView->resizeColumnToContents(ConstituentsModel::NPka3);
    ui->f_databaseView->resizeColumnToContents(ConstituentsModel::NPka2);
    ui->f_databaseView->resizeColumnToContents(ConstituentsModel::NPka1);
    ui->f_databaseView->resizeColumnToContents(ConstituentsModel::PPka1);
    ui->f_databaseView->resizeColumnToContents(ConstituentsModel::PPka2);
    ui->f_databaseView->resizeColumnToContents(ConstituentsModel::PPka3);
    ui->f_databaseView->resizeColumnToContents(ConstituentsModel::NU3);
    ui->f_databaseView->resizeColumnToContents(ConstituentsModel::NU2);
    ui->f_databaseView->resizeColumnToContents(ConstituentsModel::NU1);
    ui->f_databaseView->resizeColumnToContents(ConstituentsModel::PU1);
    ui->f_databaseView->resizeColumnToContents(ConstituentsModel::PU2);
    ui->f_databaseView->resizeColumnToContents(ConstituentsModel::PU3);
}
