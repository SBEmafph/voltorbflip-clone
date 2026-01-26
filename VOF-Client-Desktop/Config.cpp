#include "Config.h"

#include <QFile>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>

// Hilfsfunktion: Pfad zur Datei finden
QString VOF::getConfigPath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path); // Erstellt den Ordner, falls er fehlt
    return path + "/config.json";
}

// 1. Speichern (Stringify)
void VOF::saveConfig(const QString& name, quint32 id, quint16 token) {
    QJsonObject obj;
    obj["playerName"] = name;
    obj["playerID"] = static_cast<double>(id); // JSON nutzt double für Zahlen
    obj["sessionToken"] = static_cast<double>(token);

    QJsonDocument doc(obj);
    QFile file(getConfigPath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson()); // Das ist das "Stringify"
        file.close();
    }
}

// 2. Laden oder Erstellen (Parse)
QJsonObject VOF::loadOrCreateConfig() {
    QFile file(getConfigPath());

    if (!file.exists()) {
        // Datei nicht da? Erstellen mit Standardwerten
        saveConfig("NewPlayer", 0, 0);
    }

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        // Das ist das "Parse"
        return QJsonDocument::fromJson(data).object();
    }

    return QJsonObject(); // Fehlerfall
}
