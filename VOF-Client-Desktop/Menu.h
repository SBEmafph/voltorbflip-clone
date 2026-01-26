#ifndef MENU_H
#define MENU_H

#include <QMainWindow>
#include "Lobby.h"
#include "Rules.h"
#include "Match.h"
#include "Replay.h"
#include "Client.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Menu;
}
QT_END_NAMESPACE

class Menu : public QMainWindow
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);
    ~Menu();

private slots:
    void on_playBtn_clicked();    // Lobby öffnen
    void on_rulesBtn_clicked();   // Rules öffnen
    void on_replayBtn_clicked();  // Replay öffnen

private:
    Ui::Menu *ui;

    Lobby *lobby;
    rules *rulesWindow;
    Match *matchWindow;
    replay *replayWindow;
    Client *client;
};

#endif // MENU_H
