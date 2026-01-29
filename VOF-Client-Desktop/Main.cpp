#include "Menu.h"

#include <QApplication>
#include <QStyleFactory>
#include <QPalette>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle(QStyleFactory::create("Fusion"));
    a.setQuitOnLastWindowClosed(false);

    QPalette palette;
    palette.setColor(QPalette::Window, Qt::white);
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, QColor(240, 240, 240));
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, QColor(240, 240, 240));
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::Highlight, QColor(0, 120, 215));
    palette.setColor(QPalette::HighlightedText, Qt::white);

    a.setPalette(palette);

    QString profile = (argc > 1) ? argv[1] : "DefaultPlayer";
    a.setApplicationName("VoltOrbFlip_" + profile);
    QString szID = (argc > 2) ? argv[2] : 0;
    quint32 ID = szID.toInt();
    QString szToken = (argc > 3) ? argv[3] : 0;
    quint16 Token = szToken.toInt();
    QString szForce = (argc > 4) ? argv[4] : 0;
    bool Force = szForce.toInt();

    Menu w;
    w.m_setPlayerConfig(ID, Token, profile, Force);
    w.show();

    return a.exec();
}
