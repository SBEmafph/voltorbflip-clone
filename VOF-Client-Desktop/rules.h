#ifndef RULES_H
#define RULES_H

#include <QWidget>

namespace Ui
{
    class rules;
}

class rules : public QWidget
{
    Q_OBJECT

public:
    explicit rules(QWidget *parent = nullptr);
    ~rules();

signals:
    void backToMenu();

private slots:
    void on_exitBtn_clicked();

private:
    Ui::rules *ui;
};

#endif // RULES_H
