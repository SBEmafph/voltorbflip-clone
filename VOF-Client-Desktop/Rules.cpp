#include "Rules.h"
#include "ui_Rules.h"

rules::rules(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::rules)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->setAttribute(Qt::WA_ShowWithoutActivating, false);
}

rules::~rules()
{
    delete ui;
}

void rules::on_exitBtn_clicked()
{
    this->hide();
}
