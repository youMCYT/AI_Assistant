#ifndef AIWIDGET_H
#define AIWIDGET_H

#include <QWidget>

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

private:
    Ui::AIWidget *ui;
};
#endif // AIWIDGET_H
