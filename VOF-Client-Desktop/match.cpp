#include "match.h"
#include "ui_match.h"

Match::Match(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Match)
{
    ui->setupUi(this);

    // Quit-Button verbinden (falls nicht per auto-connect)
    connect(ui->quitBtn, &QPushButton::clicked,
            this, &Match::on_quitBtn_clicked);
}

Match::~Match()
{
    delete ui;
}

void Match::on_quitBtn_clicked()
{
    emit backToMenu();  // Menu informieren
    this->hide();      // Match-Fenster schließen
}
