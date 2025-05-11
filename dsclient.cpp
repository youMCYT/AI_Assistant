#include "dsclient.h"

DeepseekApiClient::DeepseekApiClient(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);

    //QString path = QDir::cleanPath(QDir::currentPath() + QDir::separator() + "setting.json");
    //qDebug() << "path:" << path;
    //getAPIKEY(path);
}

void DeepseekApiClient::sendRequest(const QString &message)
{
    QNetworkRequest request;
    request.setUrl(QUrl(API_URL));
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization", ("Bearer " + API_KEY).toUtf8());

    qDebug() << "QSslSocket:" << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "OpenSSL:" << QSslSocket::supportsSsl();

    QJsonObject json
    {
        {"model", "deepseek-chat"},
        {"messages", QJsonArray{QJsonObject{{"role", "system"}, {"content", "You are a helpful assistant."}}, QJsonObject{{"role", "user"}, {"content", message}}}}
    };

    qDebug() << "requesting...";
    qDebug() << "Request JSON:" << QJsonDocument(json).toJson();
    QNetworkReply *reply = manager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply] () {handleRequest(reply);});
}

/*
void DeepseekApiClient::getAPIKEY(const QString &path)
{
    QFile file(path);
    bool bOpen = file.open(QIODevice::ReadOnly);
    if (bOpen == false)
    {
        return;
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc["API_KEY"].isObject())
    {
        API_KEY = doc["API_KEY"].toString();
        qDebug() << "API_KEY:" << API_KEY;
    }
}
*/

void DeepseekApiClient::handleRequest(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply)
    {
        qDebug() << "successfully get reply";
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug() << reply->errorString();
            emit sendReply(reply->errorString());
            return;
        }

        QByteArray response = reply->readAll();
        qDebug() << response;
        QJsonDocument doc = QJsonDocument::fromJson(response);

        if (doc["choices"].isArray())
        {
            QString content = doc["choices"].toArray().first().toObject()["message"].toObject()["content"].toString();
            qDebug() << content;
            emit sendReply(content);
        }
        else
        {
            qDebug() << "error";
            emit sendReply("无效的响应格式");
        }

    }
}

void DeepseekApiClient::getPrompt(const QString &prompt)
{
    if (!prompt.isEmpty())
    {
        sendRequest(prompt);
    }
}
