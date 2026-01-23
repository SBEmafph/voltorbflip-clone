#ifndef LOBBY_H
#define LOBBY_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class Lobby;
}
QT_END_NAMESPACE

class Lobby : public QMainWindow
{
    Q_OBJECT

public:
    explicit Lobby(QWidget *parent = nullptr);
    ~Lobby();

signals:
    void backToMenu();
    void startMatch();   // Wechsel zu Match

private slots:
    void on_quitBtn_clicked();
    void onMatchTimeout();

private:
    Ui::Lobby *ui;
    QTimer *matchTimer;
    int secondsLeft;

protected:
    void showEvent(QShowEvent *event) override; // reset timer on sh
};

#endif // LOBBY_H
