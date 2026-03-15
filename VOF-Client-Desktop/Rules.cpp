#include "Rules.h"
#include "ui_Rules.h"

Rules::Rules(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Rules)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->setAttribute(Qt::WA_ShowWithoutActivating, false);
}

Rules::~Rules()
{
    delete ui;
}

void Rules::on_exitBtn_clicked()
{
    this->hide();
}
