#include "shop.h"
#include "ui_shop.h"

shop::shop(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::shop)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->setAttribute(Qt::WA_ShowWithoutActivating, false);

    buyButtons[0] = ui->buy1Btn;
    buyButtons[1] = ui->buy2Btn;
    buyButtons[2] = ui->buy3Btn;

    equipButtons[0] = ui->equip1Btn;
    equipButtons[1] = ui->equip2Btn;
    equipButtons[2] = ui->equip3Btn;

    for(int i = 0; i < ITEM_COUNT; ++i)
    {
        connect(buyButtons[i], &QPushButton::clicked, this, &shop::on_buyClicked);
        connect(equipButtons[i], &QPushButton::clicked, this, &shop::on_equipClicked);
    }

    updateEquipButtons();
}

shop::~shop()
{
    delete ui;
}

void shop::on_exitBtn_clicked()
{
    this->hide();
}

void shop::on_buyClicked()
{
    QPushButton* senderBtn = qobject_cast<QPushButton*>(sender());
    for(int i = 0; i < ITEM_COUNT; ++i)
    {
        if(senderBtn == buyButtons[i])
        {
            itemCount[i]++;

            switch(i)
            {
            case 0: ui->own1->setText(QString("Owned: %1").arg(itemCount[i]));
                break;
            case 1: ui->own2->setText(QString("Owned: %1").arg(itemCount[i]));
                break;
            case 2: ui->own3->setText(QString("Owned: %1").arg(itemCount[i]));
                break;
            }

            break;
        }
    }

    updateEquipButtons();
}

void shop::on_equipClicked()
{
    QPushButton* senderBtn = qobject_cast<QPushButton*>(sender());

    for(int i = 0; i < ITEM_COUNT; ++i)
    {
        if(senderBtn == equipButtons[i])
        {
            if(itemCount[i] > 0)
            {
                bool newState = !itemEquipped[i];

                for(int j = 0; j < ITEM_COUNT; ++j)
                {
                    itemEquipped[j] = false;
                    equipButtons[j]->setText("Equip-0");
                }

                if(newState)
                {
                    itemEquipped[i] = true;
                    equipButtons[i]->setText("Equip-1");
                }
            }
            break;
        }
    }
}

void shop::updateEquipButtons()
{
    for(int i = 0; i < ITEM_COUNT; ++i)
    {
        equipButtons[i]->setEnabled(itemCount[i] > 0);
    }
}
