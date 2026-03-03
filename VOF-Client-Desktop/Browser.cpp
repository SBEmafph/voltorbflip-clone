#include "Browser.h"
#include "ui_Browser.h"
#include "GlobalDefines.h"

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
    QString SZIP = ui->m_SzIP->toPlainText();
    QString SZPort = ui->m_SzPort->toPlainText();
    if (SZIP.length() >= 6 && (SZPort.length() > 2)){
        QHostAddress adress(SZIP);
        emit connectRequested(adress, SZPort.toInt());
        LOG_OUT << SZIP << " " << SZPort << " " << adress.toString();
    }
    emit connectRequested();
}

void Browser::on_ExitBtn_clicked()
{
    emit backToMenu();
}
