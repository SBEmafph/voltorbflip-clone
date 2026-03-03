#ifndef BROWSER_H
#define BROWSER_H

#include <QWidget>
#include <QTcpSocket>

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
    void backToMenu();
    void connectRequested(QHostAddress ipAdressIn = QHostAddress::LocalHost, quint16 port = 16000);

private slots:
    void on_ConnectBtn_clicked();
    void on_ExitBtn_clicked();

private:
    Ui::Browser *ui;
};

#endif // BROWSER_H
