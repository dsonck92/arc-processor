#include "arc_simulator.h"

#include <QtGui>
#include <QApplication>
#include <qstack.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //a.setStyle("windows");
    arc_simulator w;

    w.show();

    return a.exec();
}

