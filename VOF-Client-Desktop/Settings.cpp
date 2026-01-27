#include "Settings.h"
#include "ui_Settings.h"

Settings::Settings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->setAttribute(Qt::WA_ShowWithoutActivating, false);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_ExitBtn_clicked()
{
    this->hide();
}
