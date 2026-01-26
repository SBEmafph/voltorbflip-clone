#include "Rules.h"
#include "ui_Rules.h"

rules::rules(QWidget *parent) :
    QMainWindow(parent),  // wie Lobby
    ui(new Ui::rules)
{
    ui->setupUi(this);
}

rules::~rules()
{
    delete ui;
}

void rules::on_okBtn_clicked()
{
    emit backToMenu(); // Menü informieren
    this->hide();      // Rules-Fenster ausblenden
}
