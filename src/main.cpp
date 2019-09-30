#include "simul6.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(APP_NAME);
    a.setApplicationVersion(APP_VERSION);
    a.setOrganizationDomain(ORGANIZATION_DOMAIN);
    Simul6::instance()->show();
    return a.exec();
}
