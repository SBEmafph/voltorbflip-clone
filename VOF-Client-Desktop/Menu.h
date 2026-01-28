#ifndef MENU_H
#define MENU_H

#include <QMainWindow>
#include "Lobby.h"
#include "Rules.h"
#include "Match.h"
#include "Replay.h"
#include "Shop.h"
#include "Settings.h"
#include "Browser.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
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
    void on_PlayBtn_clicked();
    void on_RulesBtn_clicked();
    void on_ReplayBtn_clicked();
    void on_ShopBtn_clicked();
    void on_SettingsBtn_clicked();

    void on_BrowserConnect();

private:
    Ui::Menu *ui;

    Lobby *lobby;
    rules *rulesWindow;
    Match *matchWindow;
    replay *replayWindow;
    shop *shopWindow;
    Settings *SettingsWindow;
    Browser *BrowserWindow;
};

#endif // MENU_H
