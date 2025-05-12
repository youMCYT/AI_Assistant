#ifndef DSCLIENT_H
#define DSCLIENT_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QString>
#include <QFile>
#include <QDir>

class DeepSeekApiClient : public QObject
{
    Q_OBJECT

public:
    DeepSeekApiClient(QObject *parent = nullptr);

signals:
    void sendReply(const QString &reply, QJsonArray &dialogue);
    void findAPIKEY(bool is_APIKEY_empty, const QString &path);

private:
    QNetworkAccessManager *manager;
    QString API_KEY_path;

    QString API_KEY;
    const QString API_URL = "https://api.deepseek.com/chat/completions";

    void sendRequest(const QString &message, QJsonArray &dialogue);
    void getAPIKEY(const QString &path);

public slots:
    void getPrompt(const QString &prompt, QJsonArray &dialogue);
    void sendAPIKEY();

private slots:
    void handleRequest(QNetworkReply *reply, QJsonArray &dialogue);
};

#endif // DSCLIENT_H
