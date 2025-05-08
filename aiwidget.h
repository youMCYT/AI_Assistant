#ifndef AIWIDGET_H
#define AIWIDGET_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QTimer>
#include <QPushButton>

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

private slots:
    void sendMessage();
    void addMessage(const QString &message, bool is_self = true);

private:
    Ui::AIWidget *ui;
};

#endif // AIWIDGET_H
