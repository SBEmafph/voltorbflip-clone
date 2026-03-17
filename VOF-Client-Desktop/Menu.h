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
#include "Client.h"

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
    void m_setPlayerConfig(quint32 ID = 0, quint16 token = 0, QString name = 0, bool force = false);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_playBtn_clicked();
    void on_rulesBtn_clicked();
    void on_replayBtn_clicked();
    void on_shopBtn_clicked();
    void on_settingsBtn_clicked();
    void on_quitGameBtn_clicked();

    void slot_BrowserConnect();
    void slot_backToMenu();

private:
    Ui::Menu *ui;

    Lobby *lobby;
    Rules *rulesWindow;
    Match *matchWindow;
    replay *replayWindow;
    shop *shopWindow;
    Settings *SettingsWindow;
    Browser *browserWindow;
    Client *m_client;
    std::shared_ptr<GameState> m_pGameState;

    bool m_isQuitting = false;
};



#endif // MENU_H
