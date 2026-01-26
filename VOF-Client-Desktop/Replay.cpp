#include "Replay.h"
#include "ui_Replay.h"

replay::replay(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::replay)
{
    ui->setupUi(this);
}

replay::~replay()
{
    delete ui;
}

void replay::on_closeBtn_clicked()
{
    emit backToMenu(); // Signal senden
    this->hide();      // Replay-Fenster schließen
}
