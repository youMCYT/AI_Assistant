#ifndef AIWIDGET_H
#define AIWIDGET_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QScrollBar>
#include <QScrollArea>
#include <QWheelEvent>
#include <QLayout>
#include <QTextDocument>
#include <QMessageBox>
#include <QTextBrowser>

#include "dsclient.h"
#include "aboutdialog.h"
#include "chatdialog.h"
#include "settingdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class AIWidget;
}
QT_END_NAMESPACE

class AIWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AIWidget(QWidget *parent = nullptr);
    ~AIWidget();

signals:
    void sendDialogue(const QString &message, QJsonArray &dialogue);
    void sendChat(const QString &chat, QStringList chat_list);
    void sendSetting(const QString &pmt, const QString &m, const QString &f, const QString &key);

protected:
    //void wheelEvent(QWheelEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void sendMessage();
    void getReply(const QString &reply, bool is_error);
    void startInitWidget();

    void openDialogueWidget();
    void openSettingWidget();
    void openAboutWidget();

    void settingChanged(const QString &pmt, const QString &m, const QString &f, const QString &key);
    void chatChanged(const QString &chat);

private:
    Ui::AIWidget *ui;
    DeepSeekApiClient *cl;

    QJsonArray chat_history;
    QString history_path;

    QString setting_path;
    QString prompt = "You are a helpful assistant.";
    int font_size = 12;

    chatDialog *chat_dialog;
    settingDialog *setting_dialog;
    aboutDialog *about_dialog;

    void addMessage(const QString &message);
    void addReply(const QString &message);
    int calculateHeight();
    void setQLabelSize(QLabel *label);
    void setQTextBrowserSize(QTextBrowser *browser);
    void initChatHistory(const QString &path);
    void saveChatHistory(const QString &path);
    void initChatWidget(const QJsonArray &dialogue);
    void getSetting(const QString &path);
    void saveSetting(const QString &path);
    void getChat(const QString &path);
};

#endif // AIWIDGET_H
