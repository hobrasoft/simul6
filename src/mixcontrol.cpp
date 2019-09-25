#include "mixcontrol.h"
#include "ui_mixcontrol.h"
#include "constituentsdialog.h"
#include <QAction>


MixControl::~MixControl()
{
    delete ui;
}


MixControl::MixControl(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::MixControl)
{
    ui->setupUi(this);

    m_model = new MixControlModel(this);
    ui->f_view->setModel(m_model);

    resizeColumns();

    QAction *action;
    action = new QAction("Add", this);
    ui->f_add->setDefaultAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        ConstituentsDialog dialog;
        if (QDialog::Accepted == dialog.exec()) {
            Constituent c = dialog.constituent();
            Segments s = dialog.segments();
            QModelIndex index = m_model->add(c, s);
            Q_UNUSED(index);
            // QItemSelectionModel sm;
            // sm.select(index, QItemSelectionModel::Rows);
            // ui->f_view->setSelectionModel(&sm);
            resizeColumns();
        }
    });
    ui->f_view->addAction(action);

    action = new QAction("Remove", this);
    action->setEnabled(false);
    ui->f_remove->setDefaultAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        QModelIndex index = ui->f_view->currentIndex();
        m_model->removeRows(index.row(), 1);
    });
    connect(ui->f_view, &MyView::currentRowChanged, [this,action]() {
        action->setEnabled(ui->f_view->currentIndex().isValid());
    });
    ui->f_view->addAction(action);

    action = new QAction("Edit", this);
    action->setEnabled(false);
    ui->f_edit->setDefaultAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        int row = ui->f_view->currentIndex().row();
        ConstituentsDialog dialog;
        dialog.setConstituent(m_model->constituent(row));
        dialog.setSegments(m_model->segments(row));
        if (QDialog::Accepted == dialog.exec()) {
            Constituent c = dialog.constituent();
            Segments s = dialog.segments();
            m_model->setConstituentAndSegments(c, s, row);
            resizeColumns();
        }
    });
    connect(ui->f_view, &MyView::currentRowChanged, [this,action]() {
        action->setEnabled(ui->f_view->currentIndex().isValid());
    });
    ui->f_view->addAction(action);

    action = new QAction("Remove All", this);
    ui->f_removeAll->setDefaultAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        m_model->removeRows(0, m_model->rowCount());
    });
    ui->f_view->addAction(action);

    connect(ui->f_view, &QAbstractItemView::doubleClicked, [this](const QModelIndex& index) {
        int row = index.row();
        ConstituentsDialog dialog;
        dialog.setConstituent(m_model->constituent(row));
        dialog.setSegments(m_model->segments(row));
        if (QDialog::Accepted == dialog.exec()) {
            Constituent c = dialog.constituent();
            Segments s = dialog.segments();
            m_model->setConstituentAndSegments(c, s, row);
            resizeColumns();
        }
    });
}


void MixControl::resizeColumns() {
    for (int i=0; i<MixControlModel::LastCol; i++) {
        ui->f_view->resizeColumnToContents(i);
        }
}

