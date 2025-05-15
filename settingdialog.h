#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class settingDialog;
}
QT_END_NAMESPACE

class settingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit settingDialog(QWidget *parent = nullptr);
    ~settingDialog();

    void adjustSetting(const QString &pmt, const QString &m, const QString &f, const QString &key);

signals:
    void changeSetting(const QString &pmt, const QString &m, const QString &f, const QString &key);

private slots:
    void onSettingDialogAccepted();

private:
    Ui::settingDialog *ui;

    QString prompt;
    QString model;
    QString font_size;
    QString API_KEY;
};

#endif // SETTINGDIALOG_H
