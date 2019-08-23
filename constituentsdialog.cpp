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

    connect(ui->f_databaseView, &MyView::currentRowChanged, this, &ConstituentsDialog::currentRowChanged);

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

    int rowCount = m_constituentsProxyModel->rowCount();
    if (m_id >= 0) {
        for (int i=0; i<rowCount; i++) {
            QModelIndex index = m_constituentsProxyModel->index(i, ConstituentsModel::Id);
            int id = m_constituentsProxyModel->data(index).toInt();
            if (id == m_id) {
                ui->f_databaseView->setCurrentIndex(index);
                ui->f_databaseView->scrollTo(index);
                break;
            }
        }
    }
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


Constituent ConstituentsDialog::constituent() const {
    QVariant npka3 = m_parametersModel->data(m_parametersModel->index(ParametersModel::pKa, ParametersModel::N3));
    QVariant npka2 = m_parametersModel->data(m_parametersModel->index(ParametersModel::pKa, ParametersModel::N2));
    QVariant npka1 = m_parametersModel->data(m_parametersModel->index(ParametersModel::pKa, ParametersModel::N1));
    QVariant ppka1 = m_parametersModel->data(m_parametersModel->index(ParametersModel::pKa, ParametersModel::P1));
    QVariant ppka2 = m_parametersModel->data(m_parametersModel->index(ParametersModel::pKa, ParametersModel::P2));
    QVariant ppka3 = m_parametersModel->data(m_parametersModel->index(ParametersModel::pKa, ParametersModel::P3));
    QVariant nu3   = m_parametersModel->data(m_parametersModel->index(ParametersModel::U, ParametersModel::N3));
    QVariant nu2   = m_parametersModel->data(m_parametersModel->index(ParametersModel::U, ParametersModel::N2));
    QVariant nu1   = m_parametersModel->data(m_parametersModel->index(ParametersModel::U, ParametersModel::N1));
    QVariant pu1   = m_parametersModel->data(m_parametersModel->index(ParametersModel::U, ParametersModel::P1));
    QVariant pu2   = m_parametersModel->data(m_parametersModel->index(ParametersModel::U, ParametersModel::P2));
    QVariant pu3   = m_parametersModel->data(m_parametersModel->index(ParametersModel::U, ParametersModel::P3));

    bool manually = ui->f_manuallyGroupBox->isChecked();
    Constituent constituent(ui->f_name->text());
    constituent.setId((manually) ? 0 : m_id);

    for (;;) {
        if (npka1.isValid() && nu1.isValid()) {
            constituent.addNegU(nu1.toDouble() * Constituent::uFactor);
            constituent.addNegPKa(npka1.toDouble());
        } else {
            break;
        }
        if (npka2.isValid() && nu2.isValid()) {
            constituent.addNegU(nu2.toDouble() * Constituent::uFactor);
            constituent.addNegPKa(npka2.toDouble());
        } else {
            break;
        }
        if (npka3.isValid() && nu3.isValid()) {
            constituent.addNegU(nu3.toDouble() * Constituent::uFactor);
            constituent.addNegPKa(npka3.toDouble());
        } else {
            break;
        }
        break;
    }

    for (;;) {
        if (ppka1.isValid() && pu1.isValid()) {
            constituent.addPosU(pu1.toDouble() * Constituent::uFactor);
            constituent.addPosPKa(ppka1.toDouble());
        } else {
            break;
        }
        if (ppka2.isValid() && pu2.isValid()) {
            constituent.addPosU(pu2.toDouble() * Constituent::uFactor);
            constituent.addPosPKa(ppka2.toDouble());
        } else {
            break;
        }
        if (ppka3.isValid() && pu3.isValid()) {
            constituent.addPosU(pu3.toDouble() * Constituent::uFactor);
            constituent.addPosPKa(ppka3.toDouble());
        } else {
            break;
        }
        break;
    }

    return constituent;

}

Segments ConstituentsDialog::segments() const {
    return m_segmentsModel->segments();
}

void ConstituentsDialog::setSegments(const Segments& segments) {
    ui->f_segmentsNumber->setValue(segments.size());
    m_segmentsModel->setSegments(segments);
}


void ConstituentsDialog::setConstituent(const Constituent& constituent) {
    unsigned int negCount = constituent.getNegCount();
    unsigned int posCount = constituent.getPosCount();
    m_id 			= constituent.getId();
    QString name    = constituent.getName();
    QVariant npka3  = (negCount >= 3) ? constituent.getPKa(-3) : QVariant();
    QVariant npka2  = (negCount >= 2) ? constituent.getPKa(-2) : QVariant();
    QVariant npka1  = (negCount >= 1) ? constituent.getPKa(-1) : QVariant();
    QVariant ppka1  = (posCount >= 1) ? constituent.getPKa(1) : QVariant();
    QVariant ppka2  = (posCount >= 2) ? constituent.getPKa(2) : QVariant();
    QVariant ppka3  = (posCount >= 3) ? constituent.getPKa(3) : QVariant();
    QVariant nu3    = (negCount >= 3) ? constituent.getU(-3)/Constituent::uFactor : QVariant();
    QVariant nu2    = (negCount >= 2) ? constituent.getU(-2)/Constituent::uFactor : QVariant();
    QVariant nu1    = (negCount >= 1) ? constituent.getU(-1)/Constituent::uFactor : QVariant();
    QVariant pu1    = (posCount >= 1) ? constituent.getU(1)/Constituent::uFactor : QVariant();
    QVariant pu2    = (posCount >= 2) ? constituent.getU(2)/Constituent::uFactor : QVariant();
    QVariant pu3    = (posCount >= 3) ? constituent.getU(3)/Constituent::uFactor : QVariant();

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

    ui->f_manuallyGroupBox->setChecked(m_id <= 0);

}
