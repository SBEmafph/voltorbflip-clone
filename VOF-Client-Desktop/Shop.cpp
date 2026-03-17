#include "Shop.h"
#include "ui_Shop.h"

shop::shop(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::shop)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->setAttribute(Qt::WA_ShowWithoutActivating, false);
}

shop::~shop()
{
    delete ui;
}

void shop::on_exitBtn_clicked()
{
    this->hide();
}
