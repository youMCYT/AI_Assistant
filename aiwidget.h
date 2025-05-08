#ifndef AIWIDGET_H
#define AIWIDGET_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QScrollBar>
#include <QWheelEvent>
#include <QLayout>

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

protected:
    //void wheelEvent(QWheelEvent *event) override;

private slots:
    void sendMessage();
    void addMessage(const QString &message, bool is_self = true);

private:
    Ui::AIWidget *ui;
};

#endif // AIWIDGET_H
