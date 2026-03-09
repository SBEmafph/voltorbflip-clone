#include "Settings.h"
#include "ui_Settings.h"

#include <QRegularExpression>

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

void Settings::on_saveBtn_clicked()
{
    QString name = ui->m_SzName->text();

    if(name.isEmpty()) {
        ui->errMsg->setText("Empty Field");
        ui->m_SzName->setFocus();
        return;
    }

    if(name.length() > 15) {
        ui->errMsg->setText("Name too long");
        ui->m_SzName->setFocus();
        return;
    }

    QRegularExpression regex("^[A-Za-z]{1,15}$");

    if(regex.match(name).hasMatch())
    {
        m_playerName = name;
        ui->errMsg->clear();
        this->hide();
    }
    else
    {
        ui->errMsg->setText("Invalid Name");
        ui->m_SzName->setFocus();
    }
}

QString Settings::getPlayerName() const
{
    return m_playerName;
}
