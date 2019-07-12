#include "preferences.h"
#include "ui_preferences.h"
#include <QFileDialog>
#include <QSettings>

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);

    QSettings settings;
    ui->f_db_filename->setText(settings.value("DB/filename").toString());

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
    QSettings settings;
    settings.setValue("DB/filename", ui->f_db_filename->text());
    done(QDialog::Accepted);
}
