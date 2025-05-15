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
#include <QTextStream>

class DeepSeekApiClient : public QObject
{
    Q_OBJECT

public:
    explicit DeepSeekApiClient(QObject *parent = nullptr);
    bool tasksProcessGetter();

    void setAPIKEY(const QString &key);
    void setModel(const QString &type);

    QString getAPIKEY();
    QString getModel();

signals:
    void sendReply(const QString &reply, bool is_error);
    void runTasks(QJsonArray &dialogue);
    void taskFinished(QJsonArray &dialogue);

private:
    QNetworkAccessManager *manager;

    QString API_KEY;
    QString model;

    const QString API_URL = "https://api.deepseek.com/chat/completions";

    QQueue<QString> messages;

    bool is_task_running = false;

    void sendRequest(const QString &message, QJsonArray &dialogue);

public slots:
    void getDialogue(const QString &message, QJsonArray &dialogue);

private slots:
    void handleRequest(QNetworkReply *reply, QJsonArray &dialogue);
    void sendMultiRequest(QJsonArray &dialogue);
};

#endif // DSCLIENT_H
