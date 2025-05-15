#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include <QList>
#include <QString>
#include <QStringList>

QT_BEGIN_NAMESPACE
namespace Ui {
class chatDialog;
}
QT_END_NAMESPACE

class chatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit chatDialog(QWidget *parent = nullptr);
    ~chatDialog();

signals:
    void changeChat(const QString &chat);

public slots:
    void setChat(const QString &chat, QStringList chat_list);

private slots:
    void onChatDialogAccepted();

private:
    Ui::chatDialog *ui;

    QString history;
    QStringList histories_list;
};

#endif // CHATDIALOG_H
