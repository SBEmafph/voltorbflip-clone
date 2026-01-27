#ifndef CONFIG_H
#define CONFIG_H

#include <QJsonObject>
#include <QJsonDocument>

namespace VOF {
QString getConfigPath();
void writeConfig(quint32 id, quint16 token, const QString& name, bool force = false);
QJsonObject loadOrCreateConfig();

} // namespace VOF

#endif // CONFIG_H
