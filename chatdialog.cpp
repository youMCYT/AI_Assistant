#include "chatdialog.h"
#include "ui_chatdialog.h"

chatDialog::chatDialog(QWidget *parent): QDialog(parent), ui(new Ui::chatDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Chat");
    this->setWindowIcon(QIcon("://icon.ico"));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &chatDialog::onChatDialogAccepted);
}

chatDialog::~chatDialog()
{
    delete ui;
}

void chatDialog::setChat(const QString &chat, QStringList chat_list)
{
    history = chat;
    histories_list = chat_list;

    ui->selectComboBox->clear();
    ui->selectComboBox->addItems(chat_list);

    ui->selectComboBox->setCurrentText(history);
}

void chatDialog::onChatDialogAccepted()
{
    QString current = ui->selectComboBox->currentText();
    qDebug() << "current:" << current;

    if (ui->selectComboBox->findText(current) == -1)
    {
        ui->selectComboBox->addItem(current);
        qDebug() << "changed";
        emit changeChat(current);
    }
    else if (current != history)
    {
        qDebug() << "changed";
        emit changeChat(current);
    }
}
