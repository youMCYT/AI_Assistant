#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class aboutDialog;
}
QT_END_NAMESPACE

class aboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit aboutDialog(QWidget *parent = nullptr);
    ~aboutDialog();

private slots:
    void onAboutDialogAccepted();

private:
    Ui::aboutDialog *ui;
};

#endif // ABOUTDIALOG_H
