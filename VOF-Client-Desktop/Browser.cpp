#include "Browser.h"
#include "ui_Browser.h"

Browser::Browser(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Browser)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->setAttribute(Qt::WA_ShowWithoutActivating, false);
}

Browser::~Browser()
{
    delete ui;
}

void Browser::on_ConnectBtn_clicked()
{
    emit connectRequested();
}

void Browser::on_ExitBtn_clicked()
{
    this->hide();
}
