#include "preferences.h"
#include "ui_preferences.h"
#include <QFileDialog>
#include "msettings.h"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);

    ui->f_db_filename->setText(MSETTINGS->dbConstituents());
    ui->f_chart_antialiasing->setChecked(MSETTINGS->guiChartAntialiasing());
    ui->f_closeable->setChecked(MSETTINGS->guiCloseable());
    ui->f_moveable->setChecked(MSETTINGS->guiMoveable());
    ui->f_floatable->setChecked(MSETTINGS->guiFloatable());

    connect(ui->f_db_select, &QToolButton::clicked, [this]() {
        QString filename = QFileDialog::getOpenFileName(this,
                                     tr("Select database file"),
                                     QString(),
                                     "JSON database (*.json)"
                                     );
        if (!filename.isEmpty()) {
            ui->f_db_filename->setText(filename);
            }
        });

    connect(ui->f_set_builtin_db, &QPushButton::clicked, [this]() {
        ui->f_db_filename->setText(MSettings::DbConstituentsDefault);
        });
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::accept() {
    MSETTINGS->setDbConstituents(ui->f_db_filename->text());
    MSETTINGS->setGuiChartAntialiasing(ui->f_chart_antialiasing->isChecked());
    MSETTINGS->setGuiCloseable(ui->f_closeable->isChecked());
    MSETTINGS->setGuiMoveable(ui->f_moveable->isChecked());
    MSETTINGS->setGuiFloatable(ui->f_floatable->isChecked());

    done(QDialog::Accepted);
}
