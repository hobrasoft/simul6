/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "gpldialog.h"
#include <QFile>


GplDialog::GplDialog(QWidget *parent) : QDialog(parent) {
    setupUi(this);
    QFile file("://GPL2-license.txt");
    file.open(QIODevice::ReadOnly);
    f_editor->setPlainText(QString::fromUtf8(file.readAll()));
    file.close();
}

