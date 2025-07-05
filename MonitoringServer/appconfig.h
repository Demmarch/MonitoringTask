#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>

// Singleton
class AppConfig
{
public:
    static AppConfig& instance();

    bool loadFromFile(const QString& filePath);

    quint16 serverPort() const;
    const QString& dbHost() const;
    int dbPort() const;
    const QString& dbName() const;
    const QString& dbUser() const;
    const QString& dbPassword() const;
    int pollIntervalMs() const;

private:
    AppConfig();
    ~AppConfig();
    AppConfig(const AppConfig&) = delete;
    AppConfig& operator=(const AppConfig&) = delete;

    quint16 _serverPort;
    QString _dbHost;
    int _dbPort;
    QString _dbName;
    QString _dbUser;
    QString _dbPassword;
    int _pollIntervalMs;
};

#endif // APPCONFIG_H
