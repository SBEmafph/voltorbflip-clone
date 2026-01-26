#ifndef REPLAY_H
#define REPLAY_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class replay;
}
QT_END_NAMESPACE

class replay : public QMainWindow
{
    Q_OBJECT

public:
    explicit replay(QWidget *parent = nullptr);
    ~replay();

signals:
    void backToMenu();  // Signal zum Zurückkehren ins Menu

private slots:
    void on_closeBtn_clicked();  // Slot für Close-Button

private:
    Ui::replay *ui;
};

#endif // REPLAY_H
