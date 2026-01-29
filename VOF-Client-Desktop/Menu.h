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
    void m_setPlayerConfig(quint32 ID = 0, quint16 token = 0, QString name = 0, bool force = false);
    ~Menu();

private slots:
    void on_playBtn_clicked();     // Lobby öffnen
    void on_rulesBtn_clicked();    // Rules öffnen
    void on_replayBtn_clicked();   // Replay öffnen (NEU für menu.ui)
    void onExitBtn_clicked();

private:
    Ui::Menu *ui;

    Lobby *lobby;
    rules *rulesWindow;
    Match *matchWindow;
    replay *replayWindow;
    Client *m_client;
};

#endif // MENU_H
