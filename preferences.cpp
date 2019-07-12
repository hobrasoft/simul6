#include "preferences.h"
#include "ui_preferences.h"
#include <QFileDialog>
#include "msettings.h"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);

    ui->f_db_filename->setText(MSETTINGS->dbFilename());

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
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::accept() {
    MSETTINGS->setDbFilename(ui->f_db_filename->text());
    done(QDialog::Accepted);
}
