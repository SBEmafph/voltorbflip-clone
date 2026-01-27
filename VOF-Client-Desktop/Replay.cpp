#include "Replay.h"
#include "ui_Replay.h"

replay::replay(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::replay)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->setAttribute(Qt::WA_ShowWithoutActivating, false);
}

replay::~replay()
{
    delete ui;
}

void replay::on_exitBtn_clicked()
{
    //emit backToMenu(); // Signal senden
    this->hide();      // Replay-Fenster schließen
}
