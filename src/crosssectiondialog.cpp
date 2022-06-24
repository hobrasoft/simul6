#include "crosssectiondialog.h"
#include "ui_crosssectiondialog.h"
#include "msettings.h"
#include "computecontrol.h"


CrosssectionDialog::~CrosssectionDialog()
{
    writeSettings();
    delete ui;
}


CrosssectionDialog::CrosssectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrosssectionDialog)
{
    ComputeControl *cc = qobject_cast<ComputeControl *>(parent);
    ui->setupUi(this);
    CrosssectionModel *model = cc->crosssectionModel();
    ui->f_view->setModel(model);
    connect(ui->f_segmentsNumber, SIGNAL(valueChanged(int)), model, SLOT(setSegmentsNumber(int)));
    ui->f_segmentsNumber->setValue(model->columnCount());
    ui->f_edgeWidth->setText(QString("%1").arg(cc->crosssectionBW()));
}


void CrosssectionDialog::readSettings() {
    resize(MSETTINGS->guiCrosssectionDialogSize());
}


void CrosssectionDialog::writeSettings() {
    MSETTINGS->setGuiCrosssectionDialogSize(size());
}


void CrosssectionDialog::accept() {
    QDialog::accept();
}


void CrosssectionDialog::reject() {
    QDialog::reject();
}

