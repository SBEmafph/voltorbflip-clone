#ifndef LOBBY_H
#define LOBBY_H

#include <QWidget>
#include <QTimer>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Lobby;
}
QT_END_NAMESPACE

class Lobby : public QWidget
{
    Q_OBJECT

public:
    explicit Lobby(QWidget *parent = nullptr);
    ~Lobby();

    /** @brief Get label corresponding to slotID
    *
    *  Constructs the object name based on the slot ID and searches a matching label.
    */
    QLabel* m_getLabelBySlot(quint8 bSlotID);
    /** @brief Set Player Name
    *
    *  If the player doesnt create his own name via the settings he gets 1 of 8 assigned randomly
    */
    void m_setPlayerName(quint8 player, const QString& name);
    /** @brief Update the status of a player in the lobby
    *
    *  Get the status of the player if he is set to ready
    *  Then update ready label
    */
    void m_updatePlayerReadyStatus(quint8 bSlotID, bool isReady);

signals:
    void sig_backToMenu();
    void sig_startMatch();
    void sig_isReady();
    void sig_requestStart();

public slots:
    // Window navigation buttons
    /** @brief Start Match
    *
    *  Send ready signal
    *  The host player can force start a new Match
    */
    void on_startBtn_clicked();
    void on_quitBtn_clicked();
    void on_replayBtn_clicked();
    void on_shopBtn_clicked();

    void slot_setHostState(bool isHost);
    void slot_onMatchTimeout();
    void slot_startMatch();
    /** @brief Update Lobby depending on the Host
    *
    *  Change displayed text for the Host zu "Start Game"
    */
    void slot_updateLobby(const QString& name, quint8 bSlotID, bool fIsReady);

private:
    Ui::Lobby *ui;
    QTimer *m_matchTimer;
    int m_secondsLeft;
    bool m_fIsHost = false;
    bool m_isReady = false;
};

#endif // LOBBY_H
