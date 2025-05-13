#ifndef AIWIDGET_H
#define AIWIDGET_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QScrollBar>
#include <QScrollArea>
#include <QWheelEvent>
#include <QLayout>
#include <QTextDocument>
#include <QMessageBox>

#include "dsclient.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class AIWidget;
}
QT_END_NAMESPACE

class AIWidget : public QWidget
{
    Q_OBJECT

public:
    AIWidget(QWidget *parent = nullptr);
    ~AIWidget();

signals:
    void sendDialogue(const QString &message, QJsonArray &dialogue);
    void requestAPIKEY();

protected:
    //void wheelEvent(QWheelEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void sendMessage();
    void getReply(const QString &reply, bool is_error);
    void initWidget(bool is_APIKEY_empty, const QString &path);
    void startInitWidget();

private:
    Ui::AIWidget *ui;
    DeepSeekApiClient *cl;
    QJsonArray chat_history;
    QString history_path;

    void addMessage(const QString &message, bool is_self = true);
    int calculateHeight();
    void setQLabelHeight(QLabel *label);
    void initChatHistory(const QString &path);
    void saveChatHistory(const QString &path);
    void initChatWidget(const QJsonArray &dialogue);
};

#endif // AIWIDGET_H
