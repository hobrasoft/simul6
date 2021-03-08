#include "constituentsdialog.h"
#include "constituentsmodel.h"
#include "ui_constituentsdialog.h"
#include "segmentsdelegate.h"
#include "colorsgenerator.h"
#include "simul6.h"
#include "msettings.h"
#include "pdebug.h"
#include <QSortFilterProxyModel>
#include <QRegExp>
#include <QColorDialog>
#include <QTimer>


ConstituentsDialog::~ConstituentsDialog()
{
    writeSettings();
    delete ui;
}


ConstituentsDialog::ConstituentsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConstituentsDialog)
{
    ui->setupUi(this);
    m_constituentsModel = new ConstituentsModel(this);
    m_constituentsProxyModel = new QSortFilterProxyModel(this);
    m_constituentsProxyModel->setSourceModel(m_constituentsModel);
    ui->f_databaseView->setModel(m_constituentsProxyModel);
    connect(ui->f_databaseView->selectionModel(), &QItemSelectionModel::currentChanged, this, &ConstituentsDialog::currentDatabaseChanged);
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
    ui->f_segmentsView->setItemDelegate(new SegmentsDelegate(this));
    connect(ui->f_segmentsNumber, SIGNAL(valueChanged(int)), m_segmentsModel, SLOT(setSegmentsNumber(int)));

    connect(ui->f_databaseView, &MyView::currentRowChanged, this, &ConstituentsDialog::currentRowChanged);
    QTimer::singleShot(1, this, &ConstituentsDialog::readSettings);

    connect(ui->f_colorSelect, &QAbstractButton::clicked, [this](bool){
        m_color = QColorDialog::getColor(m_color, this);
        ui->f_color->setStyleSheet("background: "+m_color.name(QColor::HexRgb));
        });

    m_color = ColorsGenerator::color(Simul6::instance()->mixSize());
    ui->f_color->setStyleSheet("background: "+m_color.name(QColor::HexRgb));
    
}


void ConstituentsDialog::currentDatabaseChanged(const QModelIndex& current, const QModelIndex&) {
    ui->f_databaseView->scrollTo(current);
}


void ConstituentsDialog::set3FixedSegments(const QList<int>& ratios) {
    ui->f_segmentsNumber->setValue(3);
    ui->f_segmentsNumber->setReadOnly(true);
    ui->f_segmentsView->horizontalHeader()->hideSection(0);
    ui->f_segmentsView->horizontalHeader()->hideSection(2);
    ui->f_segmentsView->verticalHeader()->hideSection(0);
    ui->f_segmentsNumberWidget->setVisible(false);
    m_segmentsModel->setRatios(ratios);
}


void ConstituentsDialog::enableGroupBoxes() {
    ui->f_segmentsGroupBox->setEnabled(true);
    ui->f_databaseGroupBox->setEnabled(!manually());
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
    for (int col=0; col<m_segmentsModel->columnCount(); col++) {
        m_segmentsModel->setData(m_segmentsModel->index(SegmentsModel::Pk3n, col), npka3);
        m_segmentsModel->setData(m_segmentsModel->index(SegmentsModel::Pk2n, col), npka2);
        m_segmentsModel->setData(m_segmentsModel->index(SegmentsModel::Pk1n, col), npka1);
        m_segmentsModel->setData(m_segmentsModel->index(SegmentsModel::Pk1p, col), ppka1);
        m_segmentsModel->setData(m_segmentsModel->index(SegmentsModel::Pk2p, col), ppka2);
        m_segmentsModel->setData(m_segmentsModel->index(SegmentsModel::Pk3p, col), ppka3);
        m_segmentsModel->setData(m_segmentsModel->index(SegmentsModel::U3n, col), nu3);
        m_segmentsModel->setData(m_segmentsModel->index(SegmentsModel::U2n, col), nu2);
        m_segmentsModel->setData(m_segmentsModel->index(SegmentsModel::U1n, col), nu1);
        m_segmentsModel->setData(m_segmentsModel->index(SegmentsModel::U1p, col), pu1);
        m_segmentsModel->setData(m_segmentsModel->index(SegmentsModel::U2p, col), pu2);
        m_segmentsModel->setData(m_segmentsModel->index(SegmentsModel::U3p, col), pu3);
        }
}


bool ConstituentsDialog::manually() const {
    return ui->f_manuallyGroupBox->isChecked();
}


SegmentedConstituent ConstituentsDialog::constituent() const {
    SegmentedConstituent constituent = m_segmentsModel->constituent();
    constituent.setName(ui->f_name->text());
    constituent.setColor(m_color);
    constituent.setId((manually()) ? -1 : m_id);
    return constituent;
}


void ConstituentsDialog::setConstituent(const SegmentedConstituent& constituent) {
    // PDEBUG << constituent;
    m_id 			= constituent.getId();
    QString name    = constituent.getName();
    ui->f_segmentsNumber->setValue(constituent.segments.size());
    m_segmentsModel->setConstituent(constituent);
    m_color = constituent.color().name();
    ui->f_name->setText(name);
    ui->f_color->setStyleSheet("background: "+m_color.name(QColor::HexRgb));
    ui->f_manuallyGroupBox->setChecked(m_id < 0);
}


void ConstituentsDialog::readSettings() {
    resize(MSETTINGS->guiConstituentsDialogSize());
}


void ConstituentsDialog::writeSettings() {
    MSETTINGS->setGuiConstituentsDialogSize(size());
}


void ConstituentsDialog::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        accept();
        }
    QDialog::keyPressEvent(event);
}

