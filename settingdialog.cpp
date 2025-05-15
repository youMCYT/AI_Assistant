#include "settingdialog.h"
#include "ui_settingdialog.h"

settingDialog::settingDialog(QWidget *parent): QDialog(parent), ui(new Ui::settingDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Setting");
    this->setWindowIcon(QIcon("://icon.ico"));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &settingDialog::onSettingDialogAccepted);
}

settingDialog::~settingDialog()
{
    delete ui;
}

void settingDialog::adjustSetting(const QString &pmt, const QString &m, const QString &f, const QString &key)
{
    prompt = pmt;
    model = m;
    font_size = f;
    API_KEY = key;

    ui->keyLineEdit->setText(key);
    ui->modelComboBox->setCurrentText(m == "deepseek-chat" ? "DeepSeek V3" : "DeepSeek R1");
    ui->promptTextEdit->setPlainText(pmt);
    ui->fontSizeComboBox->setCurrentText(f);
}

void settingDialog::onSettingDialogAccepted()
{
    QString current_pmt = ui->promptTextEdit->toPlainText();
    QString current_m = ui->modelComboBox->currentText();
    QString current_f = ui->fontSizeComboBox->currentText();
    QString current_key = ui->keyLineEdit->text();

    if (current_f != font_size || current_m != model || current_pmt != prompt || current_key != API_KEY)
    {
        emit changeSetting(current_pmt, current_m, current_f, current_key);
    }
}

