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

    void m_setPlayerName(quint8 player, const QString& name);

    QLabel* m_getLabelBySlot(quint8 bSlotID);

    void m_updatePlayerReadyStatus(quint8 bSlotID, bool isReady);

signals:
    void sig_backToMenu();
    void sig_startMatch();
    void sig_isReady();
    void sig_requestStart();

public slots:
    void on_startBtn_clicked();
    void on_quitBtn_clicked();
    void on_replayBtn_clicked();
    void on_shopBtn_clicked();

    void slot_setHostState(bool isHost);
    void slot_onMatchTimeout();
    void slot_startMatch();
    void slot_updateLobby(const QString& name, quint8 bSlotID, bool fIsReady);

private:
    Ui::Lobby *ui;
    QTimer *m_matchTimer;
    int m_secondsLeft;
    bool m_fIsHost = false;
    bool m_isReady = false;


protected:
    void showEvent(QShowEvent *event) override;
};

#endif // LOBBY_H
