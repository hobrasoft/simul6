#include "simul6.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug() << "main0";
    a.setApplicationName("Simul6b");
    a.setOrganizationName("natur.cuni.cz");
    Simul6::instance()->show();
    return a.exec();
}
