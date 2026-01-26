#ifndef RULES_H
#define RULES_H

#include <QMainWindow>

namespace Ui
{
    class rules;
}

class rules : public QMainWindow
{
    Q_OBJECT

public:
    explicit rules(QWidget *parent = nullptr);
    ~rules();

signals:
    void backToMenu();

private slots:
    void on_okBtn_clicked();

private:
    Ui::rules *ui;
};

#endif // RULES_H
