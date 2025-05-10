#ifndef DSCLIENT_H
#define DSCLIENT_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>
#include <QString>

class DeepseekApiClient : public QObject
{
    Q_OBJECT

public:
    DeepseekApiClient(QObject *parent = nullptr);

signals:
    void sendReply(const QString &reply);

private:
    void handleRequest(QNetworkReply *reply);
    void sendRequest(const QString &message);

private slots:
    void getPrompt(const QString &prompt);
};

#endif // DSCLIENT_H
