#ifndef BROWSER_H
#define BROWSER_H

#include <QWidget>

namespace Ui {
class Browser;
}

class Browser : public QWidget
{
    Q_OBJECT

public:
    explicit Browser(QWidget *parent = nullptr);
    ~Browser();

signals:
    void connectRequested();

private slots:
    void on_ConnectBtn_clicked();
    void on_ExitBtn_clicked();

private:
    Ui::Browser *ui;
};

#endif // BROWSER_H
