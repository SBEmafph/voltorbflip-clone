#include "Config.h"

#include <QFile>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>

QString VOF::getConfigPath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);
    return path + "/config.json";
}

void VOF::writeConfig(quint32 id, quint16 token, const QString& name, bool force) {
    QJsonObject obj;
    obj["playerID"] = static_cast<double>(id);
    obj["sessionToken"] = static_cast<double>(token);
    obj["playerName"] = name;

    QJsonDocument doc(obj);
    QFile file(getConfigPath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

QJsonObject VOF::loadOrCreateConfig() {
    QFile file(getConfigPath());

    if (!file.exists()) {
        writeConfig(0, 0, "NewPlayer");
    }

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        return QJsonDocument::fromJson(data).object();
    }

    return QJsonObject();
}
