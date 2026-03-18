#ifndef SHOP_H
#define SHOP_H

#include <QWidget>
#include <QPushButton>

namespace Ui
{
class shop;
}

class shop : public QWidget
{
    Q_OBJECT

public:
    explicit shop(QWidget *parent = nullptr);
    ~shop();

private slots:
    void on_exitBtn_clicked();
    void on_buyClicked();
    void on_equipClicked();

private:
    Ui::shop *ui;

    static const int ITEM_COUNT = 3;

    int itemCount[ITEM_COUNT] = {0,0,0};
    bool itemEquipped[ITEM_COUNT] = {0,0,0};

    QPushButton* buyButtons[ITEM_COUNT];
    QPushButton* equipButtons[ITEM_COUNT];

    void updateEquipButtons();
};

#endif
