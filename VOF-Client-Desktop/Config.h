#ifndef CONFIG_H
#define CONFIG_H

#include <QJsonObject>
#include <QJsonDocument>

namespace VOF {
QString getConfigPath();
void saveConfig(const QString& name, quint32 id, quint16 token);
QJsonObject loadOrCreateConfig();

} // namespace VOF

#endif // CONFIG_H
