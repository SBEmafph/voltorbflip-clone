#ifndef REPLAY_H
#define REPLAY_H

#include <QWidget>
#include <QShowEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class replay;
}
QT_END_NAMESPACE

class replay : public QWidget
{
    Q_OBJECT

public:
    explicit replay(QWidget *parent = nullptr);
    ~replay();

    // Set the result of the game (won or lost) and update button color
    void setGameResult(bool won);

protected:
    // Handle window show event to enable the replay button
    void showEvent(QShowEvent *event) override;

private slots:
    void on_exitBtn_clicked();
    void onReplayBtnClicked();
    void on_importBtn_clicked();
    void on_exportBtn_clicked();

private:
    Ui::replay *ui;
    bool m_gameWon = false;
    QString buildReplayPath(int gameId) const;
    void resetAllReplays();

signals:
    void replayImported(int replayId);
};

#endif // REPLAY_H
