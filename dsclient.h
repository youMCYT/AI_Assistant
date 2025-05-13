#ifndef DSCLIENT_H
#define DSCLIENT_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QQueue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QString>
#include <QFile>
#include <QDir>
#include <QTimer>

class DeepSeekApiClient : public QObject
{
    Q_OBJECT

public:
    DeepSeekApiClient(QObject *parent = nullptr);
    bool tasksProcessGetter();

signals:
    void sendReply(const QString &reply, bool is_error);
    void findAPIKEY(bool is_APIKEY_empty, const QString &path);
    void runTasks(QJsonArray &dialogue);
    void taskFinished(QJsonArray &dialogue);

private:
    QNetworkAccessManager *manager;

    QString API_KEY_path;
    QString API_KEY;
    const QString API_URL = "https://api.deepseek.com/chat/completions";

    QQueue<QString> messages;

    bool is_task_running = false;

    void sendRequest(const QString &message, QJsonArray &dialogue);
    void getAPIKEY(const QString &path);

public slots:
    void getDialogue(const QString &message, QJsonArray &dialogue);
    void sendAPIKEY();

private slots:
    void handleRequest(QNetworkReply *reply, QJsonArray &dialogue);
    void sendMultiRequest(QJsonArray &dialogue);
};

#endif // DSCLIENT_H
