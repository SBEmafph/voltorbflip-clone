#ifndef CARDBUTTON_H
#define CARDBUTTON_H


#include <QPushButton>
#include <QGridLayout>
#include <QLabel>

class CardButton : public QPushButton {
    Q_OBJECT
public:
    explicit CardButton(int row, int col, QWidget *parent = nullptr)
        : QPushButton(parent), r(row), c(col) {
        setFixedSize(80, 80); // Größe quadratisch

        // Gitter für die 4 Notizen (1, 2, 3, Voltorb) im Button
        QGridLayout *memoLayout = new QGridLayout(this);
memoLayout->setContentsMargins(2, 2, 2, 2);
memoLayout->setSpacing(1);

for(int i = 0; i < 4; ++i) {
    memos[i] = new QLabel(this);
    memos[i]->setStyleSheet("font-size: 8px; color: yellow; border: none;");
    memos[i]->setAlignment(Qt::AlignCenter);
    memos[i]->hide(); // Standardmäßig unsichtbar
    memoLayout->addWidget(memos[i], i / 2, i % 2);
}
}

void toggleMemo(int type) { // type 1-3 für Zahlen, 4 für Voltorb
    int idx = type - 1;
    if(idx < 0 || idx > 3) return;
    if(memos[idx]->isHidden()) {
        memos[idx]->show();
        memos[idx]->setText(type == 4 ? "V" : QString::number(type));
    }
    else {
        memos[idx]->hide();
    }
}

int r, c;
QLabel* memos[4];
};

#endif // CARDBUTTON_H
