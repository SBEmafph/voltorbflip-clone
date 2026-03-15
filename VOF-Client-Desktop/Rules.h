#ifndef RULES_H
#define RULES_H

#include <QWidget>

namespace Ui
{
    class Rules;
}

class Rules : public QWidget
{
    Q_OBJECT

public:
    explicit Rules(QWidget *parent = nullptr);
    ~Rules();

private slots:
    void on_exitBtn_clicked();

private:
    Ui::Rules *ui;
};

#endif // RULES_H
