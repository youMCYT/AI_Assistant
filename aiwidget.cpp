#include "aiwidget.h"
#include "ui_aiwidget.h"

AIWidget::AIWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AIWidget)
{
    ui->setupUi(this);

    connect(ui->sendButton, &QPushButton::clicked, this, &AIWidget::sendMessage);
}

AIWidget::~AIWidget()
{
    delete ui;
}

void AIWidget::addMessage(const QString &message, bool is_self)
{
    QLabel *msgLabel = new QLabel(message);
    msgLabel->setWordWrap(true);
    msgLabel->setAlignment(is_self ? Qt::AlignRight : Qt::AlignLeft);

    ui->messageLayout->addWidget(msgLabel);
}

void AIWidget::sendMessage()
{
    QString msg = ui->inputEdit->toPlainText();

    if (!msg.isEmpty())
    {
        addMessage(msg);
        ui->inputEdit->clear();
    }
}
