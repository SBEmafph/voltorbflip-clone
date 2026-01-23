#include <QCoreApplication>

#include "voltOrbFlipServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName("VoltOrbFlipServer");
    VoltOrbFlipServer VOFserver(&a);
    //VOFserver.show();
    return a.exec();
}
