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

    void setPlayerName(const QString& name);

signals:
    void backToMenu();
    void startMatch();

private slots:
    void onMatchTimeout();
    void on_StartBtn_clicked();
    void on_quitBtn_clicked();
    void on_ReplayBtn_clicked();
    void on_ShopBtn_clicked();

private:
    Ui::Lobby *ui;
    QTimer *matchTimer;
    int secondsLeft;

protected:
    void showEvent(QShowEvent *event) override;
};

#endif // LOBBY_H
