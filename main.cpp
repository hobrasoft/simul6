#include "simul6.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Simul6b");
    a.setOrganizationName("natur.cuni.cz");
    Simul6 w;
    w.show();

    return a.exec();
}
