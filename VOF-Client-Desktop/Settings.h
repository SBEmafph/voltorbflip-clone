#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QString>

namespace Ui
{
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

    QString m_getPlayerName() const;
    void m_setPlayerName(QString sz_nameIn) {m_playerName = sz_nameIn;};

private slots:
    void on_ExitBtn_clicked();
    void on_saveBtn_clicked();

private:
    Ui::Settings *ui;
    QString m_playerName;
};

#endif // SETTINGS_H
