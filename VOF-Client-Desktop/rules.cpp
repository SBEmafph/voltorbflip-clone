#include "rules.h"
#include "ui_rules.h"

rules::rules(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::rules)
{
    ui->setupUi(this);

    // Kein echtes Popup → sonst Fokus-/Klick-Probleme
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

    // Optional: immer über dem Menü
    this->setAttribute(Qt::WA_ShowWithoutActivating, false);
}

rules::~rules()
{
    delete ui;
}

void rules::on_exitBtn_clicked()
{
    this->hide();   // Rules ausblenden
   // emit backToMenu(); // optional, falls du es weiter nutzen willst
}
