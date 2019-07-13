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
    m_constituentsModel = new ConstituentsModel(this);
    m_constituentsProxyModel = new QSortFilterProxyModel(this);
    m_constituentsProxyModel->setSourceModel(m_constituentsModel);
    ui->f_databaseView->setModel(m_constituentsProxyModel);
    connect(m_constituentsModel, &ConstituentsModel::loaded, this, &ConstituentsDialog::modelLoaded);
    connect(ui->f_manuallyGroupBox, &QGroupBox::toggled, this, &ConstituentsDialog::enableGroupBoxes);
    enableGroupBoxes();
    m_constituentsProxyModel->setFilterKeyColumn(ConstituentsModel::Name);
    m_constituentsProxyModel->sort(ConstituentsModel::Name);
    connect(ui->f_search, &QLineEdit::textEdited, [=](const QString& text) {
        m_constituentsProxyModel->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive));
    });

    m_segmentsModel = new SegmentsModel(this);
    m_segmentsModel->setSegmentsNumber(ui->f_segmentsNumber->value());
    ui->f_segmentsView->setModel(m_segmentsModel);
    connect(ui->f_segmentsNumber, SIGNAL(valueChanged(int)), m_segmentsModel, SLOT(setSegmentsNumber(int)));

    m_parametersModel = new ParametersModel(this);
    ui->f_parametersView->setModel(m_parametersModel);

    connect(ui->f_databaseView, &DatabaseView::currentRowChanged, this, &ConstituentsDialog::currentRowChanged);

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


void ConstituentsDialog::currentRowChanged(int row) {
    m_id 			= m_constituentsProxyModel->data(m_constituentsProxyModel->index(row, ConstituentsModel::Id)).toInt();
    QString name    = m_constituentsProxyModel->data(m_constituentsProxyModel->index(row, ConstituentsModel::Name)).toString();
    QVariant npka3  = m_constituentsProxyModel->data(m_constituentsProxyModel->index(row, ConstituentsModel::NPka3));
    QVariant npka2  = m_constituentsProxyModel->data(m_constituentsProxyModel->index(row, ConstituentsModel::NPka2));
    QVariant npka1  = m_constituentsProxyModel->data(m_constituentsProxyModel->index(row, ConstituentsModel::NPka1));
    QVariant ppka1  = m_constituentsProxyModel->data(m_constituentsProxyModel->index(row, ConstituentsModel::PPka1));
    QVariant ppka2  = m_constituentsProxyModel->data(m_constituentsProxyModel->index(row, ConstituentsModel::PPka2));
    QVariant ppka3  = m_constituentsProxyModel->data(m_constituentsProxyModel->index(row, ConstituentsModel::PPka3));
    QVariant nu3    = m_constituentsProxyModel->data(m_constituentsProxyModel->index(row, ConstituentsModel::NU3));
    QVariant nu2    = m_constituentsProxyModel->data(m_constituentsProxyModel->index(row, ConstituentsModel::NU2));
    QVariant nu1    = m_constituentsProxyModel->data(m_constituentsProxyModel->index(row, ConstituentsModel::NU1));
    QVariant pu1    = m_constituentsProxyModel->data(m_constituentsProxyModel->index(row, ConstituentsModel::PU1));
    QVariant pu2    = m_constituentsProxyModel->data(m_constituentsProxyModel->index(row, ConstituentsModel::PU2));
    QVariant pu3    = m_constituentsProxyModel->data(m_constituentsProxyModel->index(row, ConstituentsModel::PU3));

    ui->f_name->setText(name);
    m_parametersModel->setData(m_parametersModel->index(ParametersModel::pKa, ParametersModel::N3), npka3);
    m_parametersModel->setData(m_parametersModel->index(ParametersModel::pKa, ParametersModel::N2), npka2);
    m_parametersModel->setData(m_parametersModel->index(ParametersModel::pKa, ParametersModel::N1), npka1);
    m_parametersModel->setData(m_parametersModel->index(ParametersModel::pKa, ParametersModel::P1), ppka1);
    m_parametersModel->setData(m_parametersModel->index(ParametersModel::pKa, ParametersModel::P2), ppka2);
    m_parametersModel->setData(m_parametersModel->index(ParametersModel::pKa, ParametersModel::P3), ppka3);

    m_parametersModel->setData(m_parametersModel->index(ParametersModel::U, ParametersModel::N3), nu3);
    m_parametersModel->setData(m_parametersModel->index(ParametersModel::U, ParametersModel::N2), nu2);
    m_parametersModel->setData(m_parametersModel->index(ParametersModel::U, ParametersModel::N1), nu1);
    m_parametersModel->setData(m_parametersModel->index(ParametersModel::U, ParametersModel::P1), pu1);
    m_parametersModel->setData(m_parametersModel->index(ParametersModel::U, ParametersModel::P2), pu2);
    m_parametersModel->setData(m_parametersModel->index(ParametersModel::U, ParametersModel::P3), pu3);

}

