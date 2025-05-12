#include "aiwidget.h"
#include "ui_aiwidget.h"

AIWidget::AIWidget(QWidget *parent): QWidget(parent), ui(new Ui::AIWidget)
{
    ui->setupUi(this);
    cl = new DeepSeekApiClient(this);
    history_path = QDir::cleanPath(QDir::currentPath() + QDir::separator() + QString("history.json"));
    initChatHistory(history_path);

    ui->scrollArea->setWidgetResizable(false);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->scrollArea->viewport()->installEventFilter(this);

    ui->messageContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    connect(ui->sendButton, &QPushButton::clicked, this, &AIWidget::sendMessage);

    connect(cl, &DeepSeekApiClient::sendReply, this, &AIWidget::getReply);
    connect(this, &AIWidget::sendPrompt, cl, &DeepSeekApiClient::getPrompt);
    connect(cl, &DeepSeekApiClient::findAPIKEY, this, &AIWidget::initWidget);
    connect(this, &AIWidget::requestAPIKEY, cl, &DeepSeekApiClient::sendAPIKEY);

    startInitWidget();
}

AIWidget::~AIWidget()
{
    delete ui;
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

bool AIWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->scrollArea->viewport() && event->type() == QEvent::Resize)
    {
        ui->messageContainer->setFixedWidth(ui->scrollArea->viewport()->width());
        qDebug() << "width:" << ui->messageContainer->width();

        ui->messageContainer->setFixedHeight(calculateHeight());
    }
    return QWidget::eventFilter(watched, event);
}

void AIWidget::handleAppExit()
{
    saveChatHistory(history_path);
}

void AIWidget::sendMessage()
{
    QString msg = ui->inputEdit->toPlainText();

    if (!msg.isEmpty())
    {
        addMessage(msg);
        ui->inputEdit->clear();
        emit sendPrompt(msg, chat_history);
    }
}

void AIWidget::getReply(const QString &reply, QJsonArray &dialogue)
{
    if (!reply.isEmpty())
    {
        addMessage(reply, false);

        dialogue.append(QJsonObject{{"role", "assistant"}, {"content", reply}});
    }
}

void AIWidget::initWidget(bool is_APIKEY_empty, const QString &path)
{
    if (is_APIKEY_empty)
    {
        addMessage(QString("请使用支持的编辑器打开setting.json文件，依据文件内的指引添加你的DeepSeek API KEY，并在完成后重启程序。\n文件路径：") + path, false);
    }
    else
    {
        initChatWidget(chat_history);
    }
}

void AIWidget::addMessage(const QString &message, bool is_self)
{
    QLabel *msg_label = new QLabel(message);
    msg_label->setWordWrap(true);
    msg_label->setAlignment(is_self ? (Qt::AlignRight | Qt::AlignTop) : (Qt::AlignLeft | Qt::AlignTop));
    msg_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setQLabelHeight(msg_label);

    ui->messageLayout->addWidget(msg_label);

    ui->messageContainer->setFixedHeight(calculateHeight());
}

int AIWidget::calculateHeight()
{
    int total_height = ui->messageContainer->layout()->contentsMargins().bottom() + ui->messageContainer->layout()->contentsMargins().top() - ui->messageLayout->spacing();

    for (int i = 0; i < ui->messageLayout->count(); i++)
    {
        QWidget *widget_object = ui->messageLayout->itemAt(i)->widget();
        if (widget_object)
        {
            if (QLabel *label = qobject_cast<QLabel*>(widget_object))
            {
                setQLabelHeight(label);
            }
            total_height += widget_object->height() + ui->messageLayout->spacing();
        }
    }
    qDebug() << "height:" << total_height;

    return total_height;
}

void AIWidget::setQLabelHeight(QLabel *label)
{
    QTextDocument doc;

    doc.setDefaultFont(label->font());
    doc.setPlainText(label->text());
    doc.setTextWidth(label->width());
    label->setFixedHeight(doc.size().height() + 2 * label->margin());
}

void AIWidget::initChatHistory(const QString &path)
{
    QFile file(path);

    if (!QFile::exists(path))
    {
        file.open(QIODevice::WriteOnly);
        file.write(QJsonDocument(QJsonArray{QJsonObject{{"role", "system"}, {"content", "You are a helpful assistant."}}}).toJson());
        file.flush();
        file.close();
    }

    file.open(QIODevice::ReadOnly);

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    chat_history = doc.array();
}

void AIWidget::saveChatHistory(const QString &path)
{
    QFile file(path);

    file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(chat_history).toJson());
    file.flush();
    file.close();
}

void AIWidget::initChatWidget(const QJsonArray &dialogue)
{
    for (int i = 0; i < dialogue.count(); i++)
    {
        qDebug() << "role:" << dialogue.at(i).toObject()["role"].toString() << "/ncontent:" << dialogue.at(i).toObject()["content"].toString();
        if (dialogue.at(i).toObject()["role"].toString() == "assistant")
        {
            addMessage(dialogue.at(i).toObject()["content"].toString(), false);
        }
        else if (dialogue.at(i).toObject()["role"].toString() == "user")
        {
            addMessage(dialogue.at(i).toObject()["content"].toString());
        }
    }
}

void AIWidget::startInitWidget()
{
    emit requestAPIKEY();
}
