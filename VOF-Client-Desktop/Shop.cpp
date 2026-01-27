#include "Shop.h"
#include "ui_Shop.h"

shop::shop(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::shop)
{
    ui->setupUi(this);
}

shop::~shop()
{
    delete ui;
}
