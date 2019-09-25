/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "ampholines.h"
#include "pdebug.h"
#include "ui_ampholines.h"

Ampholines::Ampholines(QWidget *parent) : QDialog(parent), ui(new Ui::Ampholines) {
    ui->setupUi(this);

    m_segmentsModel = new SegmentsModel(this);
    m_segmentsModel->setSegmentsNumber(ui->f_segmentsNumber->value());
    ui->f_segmentsView->setModel(m_segmentsModel);
    connect(ui->f_segmentsNumber, SIGNAL(valueChanged(int)), m_segmentsModel, SLOT(setSegmentsNumber(int)));


    connect(ui->f_low_pKAn,  QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Ampholines::recalculate);
    connect(ui->f_low_pKAp,  QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Ampholines::recalculate);
    connect(ui->f_high_pKAn, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Ampholines::recalculate);
    connect(ui->f_number,    QOverload<int>::of(&QSpinBox::valueChanged), this, &Ampholines::recalculate);

}


void Ampholines::recalculate() {
    ui->f_low_pKAp->setMaximum(ui->f_low_pKAn->value());
    double delta = (ui->f_high_pKAn->value() - ui->f_low_pKAn->value()) / (ui->f_number->value() - 1);
    double pkap  = ui->f_low_pKAp->value() + (ui->f_number->value() - 1) * delta;
    ui->f_delta_pKA->setText(QString("%1").arg(delta, 0, 'g', 3, QChar('0')));
    ui->f_high_pKAp->setText(QString("%1").arg(pkap, 0, 'g', 3, QChar('0')));
}


void Ampholines::accept() {
    double delta = (ui->f_high_pKAn->value() - ui->f_low_pKAn->value()) / (ui->f_number->value() - 1);
    double low_pKAn = ui->f_low_pKAn->value();
    double low_pKAp = ui->f_low_pKAp->value();
    double Un = ui->f_Un->value();
    double Up = ui->f_Up->value();
    for (int i=0; i<ui->f_number->value(); i++) {
        double pKAp = low_pKAp + i * delta;
        double pKAn = low_pKAn + i * delta;
        QString name = QString("%1%2").arg(ui->f_name->text()).arg(i, 4, 10, QChar('0'));
        PDEBUG << name << pKAn << pKAp << Un << Up;
        Constituent constituent(name);
        constituent.addNegPKa(pKAn);
        constituent.addPosPKa(pKAp);
        constituent.addNegU(Un * Constituent::uFactor);
        constituent.addPosU(Up * Constituent::uFactor);
        m_constituents << constituent;
        m_segments << m_segmentsModel->segments();
        }
    QDialog::accept();
}

