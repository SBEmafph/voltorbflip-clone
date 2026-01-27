#ifndef REPLAY_H
#define REPLAY_H

#include <QWidget>

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

private slots:
    void on_exitBtn_clicked();  // Slot für Close-Button

private:
    Ui::replay *ui;
};

#endif // REPLAY_H
