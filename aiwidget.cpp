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
    QLabel *msg_label = new QLabel(message);
    msg_label->setWordWrap(true);
    msg_label->setAlignment(is_self ? (Qt::AlignRight | Qt::AlignTop) : (Qt::AlignLeft | Qt::AlignTop));
    msg_label->adjustSize();

    int total_height = ui->messageContainer->layout()->contentsMargins().bottom() + ui->messageContainer->layout()->contentsMargins().top();

    for (int i = 0; i < ui->messageLayout->count(); i++)
    {
        QWidget *w = ui->messageLayout->itemAt(i)->widget();
        if (w)
        {
            total_height += w->sizeHint().height() + ui->messageLayout->spacing();
            qDebug() << total_height;
        }
    }

    total_height += msg_label->sizeHint().height();
    qDebug() << total_height;
    ui->messageContainer->setFixedHeight(total_height);

    ui->messageLayout->addWidget(msg_label);
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
/*
void AIWidget::wheelEvent(QWheelEvent *event)
{
    QScrollArea *is_scrollArea = findChild<QScrollArea*>("scrollArea");

    if(is_scrollArea)
    {
        QScrollBar *vScroll = ui->scrollArea->verticalScrollBar();
        int newValue = vScroll->value() - event->angleDelta().y() / 8;
        newValue = qMax(vScroll->minimum(), qMin(newValue, vScroll->maximum()));
        vScroll->setValue(newValue);
        event->accept();
    }
    else
    {
        QWidget::wheelEvent(event);
    }
}
*/
