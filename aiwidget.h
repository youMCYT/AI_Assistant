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
    void sendPrompt(const QString &prompt);

protected:
    //void wheelEvent(QWheelEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void sendMessage();
    void getReply(const QString &reply);

private:
    Ui::AIWidget *ui;
    void addMessage(const QString &message, bool is_self = true);
    int calculateHeight();
    void setQLabelHeight(QLabel *label);
};

#endif // AIWIDGET_H
