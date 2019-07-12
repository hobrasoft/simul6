#include "mixcontrol.h"
#include "ui_mixcontrol.h"
#include "constituentsdialog.h"
#include <QAction>

MixControl::MixControl(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::MixControl)
{
    ui->setupUi(this);

    m_model = new MixControlModel(this);
    ui->f_view->setModel(m_model);
    for (int i=0; i<MixControlModel::LastCol; i++) {
        ui->f_view->resizeColumnToContents(i);
    }

    QAction *action;
    action = new QAction("Add", this);
    ui->f_add->setDefaultAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        ConstituentsDialog dialog;
        dialog.exec();
    });

    action = new QAction("Remove", this);
    ui->f_remove->setDefaultAction(action);

    action = new QAction("Edit", this);
    ui->f_edit->setDefaultAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        ConstituentsDialog dialog;
        dialog.exec();
    });
}

MixControl::~MixControl()
{
    delete ui;
}
