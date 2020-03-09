/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "localurl.h"
#include "gpldialog.h"
#include "pdebug.h"
#include <QDesktopServices>

LocalUrl::LocalUrl(QObject *parent) : QObject(parent) {
    PDEBUG;
    QDesktopServices::setUrlHandler("local", this, "showUrl");
}

void LocalUrl::showUrl(const QUrl& url) {
    PDEBUG << url.toString() << url.path();
    if (url.path() == "/gpl2") {
        GplDialog gpldialog(qobject_cast<QWidget *>(parent()));
        gpldialog.exec();
        }
}

