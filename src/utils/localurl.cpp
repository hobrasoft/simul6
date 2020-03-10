/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "localurl.h"
#include "gpldialog.h"
#include <QDesktopServices>

LocalUrl::LocalUrl(QObject *parent) : QObject(parent) {
    QDesktopServices::setUrlHandler("local", this, "showUrl");
}

void LocalUrl::showUrl(const QUrl& url) {
    if (url.path() == "/gpl2") {
        GplDialog gpldialog(qobject_cast<QWidget *>(parent()));
        gpldialog.exec();
        }
}

