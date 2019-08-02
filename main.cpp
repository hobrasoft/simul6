#include "simul6.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug() << "main0";
    a.setApplicationName("Simul6b");
    a.setOrganizationName("natur.cuni.cz");
    Simul6 w;
qDebug() << "main1";
    w.show();
qDebug() << "main2";
    return a.exec();
}
