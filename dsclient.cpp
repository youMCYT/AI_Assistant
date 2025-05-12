#include "dsclient.h"

DeepSeekApiClient::DeepSeekApiClient(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    API_KEY_path = QDir::cleanPath(QDir::currentPath() + QDir::separator() + QString("setting.json"));

    getAPIKEY(API_KEY_path);
}

void DeepSeekApiClient::sendRequest(const QString &message, QJsonArray &dialogue)
{
    QNetworkRequest request;
    request.setUrl(QUrl(API_URL));
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization", ("Bearer " + API_KEY).toUtf8());

    qDebug() << "QSslSocket:" << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "OpenSSL:" << QSslSocket::supportsSsl();

    dialogue.append(QJsonObject{{"role", "user"}, {"content", message}});

    QJsonObject json
    {
        {"model", "deepseek-chat"},
        {"messages", dialogue}
    };

    qDebug() << "requesting...";
    qDebug() << "Request JSON:" << QJsonDocument(json).toJson();
    QNetworkReply *reply = manager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply, &dialogue] () {
        handleRequest(reply, dialogue);
        reply->deleteLater();
    });
}

void DeepSeekApiClient::getAPIKEY(const QString &path)
{
    QFile file(path);

    if (!QFile::exists(path))
    {
        file.open(QIODevice::WriteOnly);
        file.write(QJsonDocument(QJsonObject{{"API_KEY", "将引号内的文字替换为你的DeepSeek API KEY"}}).toJson());
        file.flush();
        file.close();
    }

    file.open(QIODevice::ReadOnly);

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject json = doc.object();

    if (json["API_KEY"].toString() != "将引号内的文字替换为你的DeepSeek API KEY")
    {
        API_KEY = json["API_KEY"].toString();
    }
}

void DeepSeekApiClient::getPrompt(const QString &prompt, QJsonArray &dialogue)
{
    if (!prompt.isEmpty())
    {
        sendRequest(prompt, dialogue);
    }
}

void DeepSeekApiClient::sendAPIKEY()
{
    emit findAPIKEY(API_KEY.isEmpty(), API_KEY_path);
}

void DeepSeekApiClient::handleRequest(QNetworkReply *reply, QJsonArray &dialogue)
{
    if (reply)
    {
        qDebug() << "successfully get reply";
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug() << reply->errorString();
            emit sendReply(reply->errorString(), dialogue);
            return;
        }

        QByteArray response = reply->readAll();
        qDebug() << response;
        QJsonDocument doc = QJsonDocument::fromJson(response);

        if (doc["choices"].isArray())
        {
            QString content = doc["choices"].toArray().first().toObject()["message"].toObject()["content"].toString();
            qDebug() << content;
            emit sendReply(content, dialogue);
        }
        else
        {
            qDebug() << "error";
            emit sendReply("无效的响应格式", dialogue);
        }

    }
}
