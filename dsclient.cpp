#include "dsclient.h"

DeepSeekApiClient::DeepSeekApiClient(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);

    connect(this, &DeepSeekApiClient::runTasks, this, &DeepSeekApiClient::sendMultiRequest);
    connect(this, &DeepSeekApiClient::taskFinished, this, &DeepSeekApiClient::sendMultiRequest);
}

bool DeepSeekApiClient::tasksProcessGetter()
{
    return is_task_running;
}

void DeepSeekApiClient::setAPIKEY(const QString &key)
{
    API_KEY = key;
}

void DeepSeekApiClient::setModel(const QString &type)
{
    model = type;
}

QString DeepSeekApiClient::getAPIKEY()
{
    return API_KEY;
}

QString DeepSeekApiClient::getModel()
{
    return model;
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

    QJsonArray di = dialogue;

    for(QJsonArray::Iterator it = di.begin(); it != di.end(); it++)
    {
        QJsonObject obj = it[0].toObject();
        it[0] = QJsonObject{{"role", obj["role"]}, {"content", obj["content"]}};
    }

    QJsonObject json
    {
        {"model", model},
        {"messages", di}
    };

    qDebug() << "requesting...";
    qDebug() << "Request JSON:" << QJsonDocument(json).toJson();
    QNetworkReply *reply = manager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply, &dialogue] () {
        handleRequest(reply, dialogue);
        reply->deleteLater();
        emit taskFinished(dialogue);
    });
}

void DeepSeekApiClient::getDialogue(const QString &message, QJsonArray &dialogue)
{
    messages.enqueue(message);
    if (!is_task_running)
    {
        emit runTasks(dialogue);
    }
}

void DeepSeekApiClient::handleRequest(QNetworkReply *reply, QJsonArray &dialogue)
{
    if (reply)
    {
        qDebug() << "successfully get reply";
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug() << reply->errorString();
            emit sendReply(reply->errorString(), true);
            return;
        }

        QByteArray response = reply->readAll();
        qDebug() << response;
        QJsonDocument doc = QJsonDocument::fromJson(response);

        if (doc["choices"].isArray())
        {
            if (model == "deepseek-chat")
            {
                QString content = doc["choices"].toArray().first().toObject()["message"].toObject()["content"].toString();
                qDebug() << content;
                dialogue.append(QJsonObject{{"role", "assistant"}, {"content", content}});
                emit sendReply(content, false);
            }
            else
            {
                QString content = doc["choices"].toArray().first().toObject()["message"].toObject()["content"].toString();
                QString reasoning_content = doc["choices"].toArray().first().toObject()["message"].toObject()["reasoning_content"].toString();
                QString msg = reasoning_content;
                msg.append("\n\n---\n\n");
                msg.append(content);
                qDebug() << msg;
                dialogue.append(QJsonObject{{"role", "assistant"}, {"content", content}, {"reasoning_content", reasoning_content}});
                emit sendReply(msg, false);
            }
        }
        else
        {
            qDebug() << "error";
            emit sendReply("无效的响应格式", true);
        }
    }
}

void DeepSeekApiClient::sendMultiRequest(QJsonArray &dialogue)
{
    if (messages.isEmpty())
    {
        is_task_running = false;
    }
    else
    {
        is_task_running = true;
        const QString message = messages.head();
        qDebug() << dialogue << "\n" << message;
        messages.dequeue();

        QTimer::singleShot(0, this, [this, message, &dialogue] () {
            sendRequest(message, dialogue);
        });
    }
}
