#include "aiwidget.h"
#include "ui_aiwidget.h"

AIWidget::AIWidget(QWidget *parent): QWidget(parent), ui(new Ui::AIWidget)
{
    ui->setupUi(this);
    this->setWindowTitle("AI Assistant");
    this->setWindowIcon(QIcon("://icon.ico"));

    cl = new DeepSeekApiClient(this);

    history_path = QDir::cleanPath(QDir::currentPath() + QDir::separator() + QString("history.json"));
    setting_path = QDir::cleanPath(QDir::currentPath() + QDir::separator() + QString("setting.json"));

    initChatHistory(history_path);

    setting_dialog = new settingDialog(this);
    setting_dialog->setModal(true);
    setting_dialog->hide();
    connect(setting_dialog, &settingDialog::changeSetting, this, &AIWidget::settingChanged);
    connect(this, &AIWidget::sendSetting, setting_dialog, &settingDialog::adjustSetting);
    QTimer::singleShot(0, this, [this] () {
        getSetting(setting_path);
    });

    chat_dialog = new chatDialog(this);
    chat_dialog->setModal(true);
    chat_dialog->hide();
    connect(chat_dialog, &chatDialog::changeChat, this, &AIWidget::chatChanged);
    connect(this, &AIWidget::sendChat, chat_dialog, &chatDialog::setChat);
    QTimer::singleShot(0, this, [this] () {
        getChat(history_path);
    });

    about_dialog = new aboutDialog(this);
    about_dialog->setModal(true);
    about_dialog->hide();

    ui->scrollArea->setWidgetResizable(false);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->scrollArea->viewport()->installEventFilter(this);
    qDebug() << ui->scrollArea->viewport()->width();

    ui->messageContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    connect(ui->sendButton, &QPushButton::clicked, this, &AIWidget::sendMessage);
    connect(ui->chatButton, &QPushButton::clicked, this, &AIWidget::openDialogueWidget);
    connect(ui->settingButton, &QPushButton::clicked, this, &AIWidget::openSettingWidget);
    connect(ui->aboutButton, &QPushButton::clicked, this, &AIWidget::openAboutWidget);

    connect(cl, &DeepSeekApiClient::sendReply, this, &AIWidget::getReply);
    connect(this, &AIWidget::sendDialogue, cl, &DeepSeekApiClient::getDialogue);

    QTimer::singleShot(0, this, &AIWidget::startInitWidget);
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

void AIWidget::closeEvent(QCloseEvent *event)
{
    if (cl->tasksProcessGetter())
    {
        QMessageBox::information(this, "提示", "还有尚未执行完毕的请求！\n请等待所有请求执行完毕...");
        event->ignore();
    }
    else
    {
        saveChatHistory(history_path);
        saveSetting(setting_path);
        event->accept();
    }
}

void AIWidget::sendMessage()
{
    QString msg = ui->inputEdit->toPlainText();

    if (!msg.isEmpty())
    {
        addMessage(msg);
        ui->inputEdit->clear();
        emit sendDialogue(msg, chat_history);
    }
}

void AIWidget::getReply(const QString &reply, bool is_error)
{
    if (!reply.isEmpty())
    {
        if (is_error)
        {
            chat_history.removeLast();
        }

        addReply(reply);
    }
}

void AIWidget::startInitWidget()
{
    initChatWidget(chat_history);
}

void AIWidget::openDialogueWidget()
{
    if (cl->tasksProcessGetter())
    {
        QMessageBox::information(this, "提示", "还有尚未执行完毕的请求！\n请等待所有请求执行完毕...");
    }
    else
    {
        chat_dialog->show();
    }
}

void AIWidget::openSettingWidget()
{
    if (cl->tasksProcessGetter())
    {
        QMessageBox::information(this, "提示", "还有尚未执行完毕的请求！\n请等待所有请求执行完毕...");
    }
    else
    {
        setting_dialog->show();
    }
}

void AIWidget::openAboutWidget()
{
    if (cl->tasksProcessGetter())
    {
        QMessageBox::information(this, "提示", "还有尚未执行完毕的请求！\n请等待所有请求执行完毕...");
    }
    else
    {
        about_dialog->show();
    }
}

void AIWidget::settingChanged(const QString &pmt, const QString &m, const QString &f, const QString &key)
{
    prompt = pmt;
    font_size = f.toInt();
    cl->setAPIKEY(key);
    cl->setModel(m == "DeepSeek V3" ? "deepseek-chat" : "deepseek-reasoner");

    saveSetting(setting_path);

    QJsonArray::Iterator it = chat_history.begin();
    QJsonObject pm = it[0].toObject();
    pm["content"] = prompt;
    it[0] = pm;

    ui->messageContainer->setFixedHeight(calculateHeight());
}

void AIWidget::chatChanged(const QString &chat)
{
    saveChatHistory(history_path);

    QFile file(history_path);

    file.open(QIODevice::ReadOnly);

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject doc_object = doc.object();
    QStringList chats = doc_object["history"].toObject().keys();

    doc_object["current"] = chat;
    if (!chats.contains(chat))
    {
        QJsonObject history = doc_object["history"].toObject();
        history[chat] = QJsonArray{QJsonObject{{"role", "system"}, {"content", prompt}}};
        doc_object["history"] = history;
        chats.append(chat);
    }
    doc.setObject(doc_object);

    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson());
    file.flush();
    file.close();

    initChatHistory(history_path);
    QJsonArray::Iterator it = chat_history.begin();
    QJsonObject pmt = it[0].toObject();
    pmt["content"] = prompt;
    it[0] = pmt;

    for (int i = 0; i < ui->messageLayout->count(); i++)
    {
        QWidget *widget_object = ui->messageLayout->itemAt(i)->widget();
        if (widget_object)
        {
            QTimer::singleShot(0, this, [this, widget_object] () {
                ui->messageLayout->removeWidget(widget_object);
                widget_object->deleteLater();
            });
        }
    }

    QTimer::singleShot(0, this, &AIWidget::startInitWidget);

    emit sendChat(chat, chats);
}

void AIWidget::addMessage(const QString &message)
{
    QLabel *msg_label = new QLabel(message, this);

    QFont label_font = msg_label->font();
    label_font.setPointSize(font_size);
    msg_label->setFont(label_font);

    msg_label->setWordWrap(true);
    msg_label->setTextInteractionFlags(Qt::TextSelectableByMouse);

    msg_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    ui->messageLayout->addWidget(msg_label, 0, Qt::AlignRight);
    qDebug() << msg_label->width();

    QWidget *msgContainer = ui->messageContainer;

    QTimer::singleShot(0, this, [this, msgContainer] () {
        msgContainer->setFixedHeight(calculateHeight());
    });
}

void AIWidget::addReply(const QString &message)
{
    QTextBrowser *msg_browser = new QTextBrowser(this);

    QFont browser_font = msg_browser->font();
    browser_font.setPointSize(font_size);
    msg_browser->setFont(browser_font);

    msg_browser->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    msg_browser->setLineWrapMode(QTextEdit::NoWrap);
    msg_browser->setWordWrapMode(QTextOption::NoWrap);

    msg_browser->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);

    msg_browser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    msg_browser->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    msg_browser->setMarkdown(message);
    ui->messageLayout->addWidget(msg_browser, 0, Qt::AlignLeft);

    QWidget *msgContainer = ui->messageContainer;

    QTimer::singleShot(0, this, [this, msgContainer] () {
        msgContainer->setFixedHeight(calculateHeight());
    });
}

int AIWidget::calculateHeight()
{
    int total_height = ui->messageContainer->layout()->contentsMargins().bottom() + ui->messageContainer->layout()->contentsMargins().top() - ui->messageLayout->spacing();

    for (int i = 0; i < ui->messageLayout->count(); i++)
    {
        QWidget *widget_object = ui->messageLayout->itemAt(i)->widget();
        if (widget_object)
        {
            if (QTextBrowser *browser = qobject_cast<QTextBrowser*>(widget_object))
            {
                setQTextBrowserSize(browser);
            }
            else if (QLabel *label = qobject_cast<QLabel*>(widget_object))
            {
                setQLabelSize(label);
            }
            total_height += widget_object->height() + ui->messageLayout->spacing();
        }
    }
    qDebug() << "height:" << total_height;

    return total_height;
}

void AIWidget::setQLabelSize(QLabel *label)
{
    QTextDocument doc;

    QFont label_font = label->font();
    label_font.setPointSize(font_size);
    label->setFont(label_font);
    doc.setDefaultFont(label_font);

    doc.setPlainText(label->text());

    int label_width = doc.idealWidth() + 2 * label->margin();
    int widget_width = ui->messageContainer->width();
    int widget_margin = ui->messageContainer->layout()->contentsMargins().left() + ui->messageContainer->layout()->contentsMargins().right();

    label->setFixedWidth(label_width > widget_width - widget_margin
                             ? widget_width - widget_margin
                             : label_width);
    doc.setTextWidth(label->width());
    label->setFixedHeight(doc.size().height() + 2 * label->margin());
}

void AIWidget::setQTextBrowserSize(QTextBrowser *browser)
{
    QFont browser_font = browser->document()->defaultFont();
    browser_font.setPointSize(font_size);
    browser->document()->setDefaultFont(browser_font);

    int browser_width = browser->document()->idealWidth() + 2 * browser->document()->documentMargin();
    int widget_width = ui->messageContainer->width();
    int widget_margin = ui->messageContainer->layout()->contentsMargins().left() + ui->messageContainer->layout()->contentsMargins().right();

    browser->setFixedWidth(browser_width > widget_width - widget_margin
                               ? widget_width - widget_margin
                               : browser_width);
    browser->setFixedHeight(browser->document()->size().height() + browser->contentsMargins().top() + browser->contentsMargins().bottom() + (browser->horizontalScrollBar()->isVisible() ? browser->horizontalScrollBar()->height() : 0));
}

void AIWidget::initChatHistory(const QString &path)
{
    QFile file(path);

    if (!QFile::exists(path))
    {
        file.open(QIODevice::WriteOnly);
        QJsonObject json
        {
            {"current", "default"},
            {"history", QJsonObject{{"default", QJsonArray{QJsonObject{{"role", "system"}, {"content", prompt}}}}}}
        };

        file.write(QJsonDocument(json).toJson());
        file.flush();
        file.close();
    }

    file.open(QIODevice::ReadOnly);

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject doc_object = doc.object();
    qDebug() << doc_object["current"].toString();

    chat_history = doc_object["history"].toObject()[doc["current"].toString()].toArray();
}

void AIWidget::saveChatHistory(const QString &path)
{
    QFile file(path);

    file.open(QIODevice::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    qDebug() << doc;

    QJsonObject json = doc.object();
    QJsonObject history = json["history"].toObject();
    history[json["current"].toString()] = chat_history;
    json["history"] = history;
    doc.setObject(json);

    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson());
    file.flush();
    file.close();
}

void AIWidget::initChatWidget(const QJsonArray &dialogue)
{
    ui->messageContainer->setFixedWidth(ui->scrollArea->viewport()->width());
    qDebug() << "width:" << ui->messageContainer->width();

    for (int i = 0; i < dialogue.count(); i++)
    {
        qDebug() << "role:" << dialogue.at(i).toObject()["role"].toString() << "\ncontent:" << dialogue.at(i).toObject()["content"].toString();
        if (dialogue.at(i).toObject()["role"].toString() == "assistant")
        {
            QString re;

            if (dialogue.at(i).toObject().contains("reasoning_content"))
            {
                re = dialogue.at(i).toObject()["reasoning_content"].toString();
                re.append("\n\n---\n\n");
                re.append(dialogue.at(i).toObject()["content"].toString());
            }
            else
            {
                re = dialogue.at(i).toObject()["content"].toString();
            }

            addReply(re);
        }
        else if (dialogue.at(i).toObject()["role"].toString() == "user")
        {
            addMessage(dialogue.at(i).toObject()["content"].toString());
        }
    }

    ui->messageContainer->setFixedHeight(calculateHeight());
}

void AIWidget::getSetting(const QString &path)
{
    QFile file(path);

    if (!QFile::exists(path))
    {
        file.open(QIODevice::WriteOnly);
        file.write(QJsonDocument(QJsonObject{{"API_KEY", ""}, {"model", "deepseek-chat"}, {"prompt", prompt}, {"font", 12}}).toJson());
        file.flush();
        file.close();
    }

    file.open(QIODevice::ReadOnly);

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject json = doc.object();

    if (json["API_KEY"].toString() != "")
    {
        cl->setAPIKEY(json["API_KEY"].toString());
    }

    if (json["prompt"].toString() != "")
    {
        prompt = json["prompt"].toString();
    }

    QJsonArray::Iterator it = chat_history.begin();
    QJsonObject pmt = it[0].toObject();
    pmt["content"] = prompt;
    it[0] = pmt;

    cl->setModel(json["model"].toString());
    font_size = json["font"].toInt();

    emit sendSetting(prompt, cl->getModel(), QString::number(font_size), cl->getAPIKEY());
}

void AIWidget::saveSetting(const QString &path)
{
    QFile file(path);

    file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(QJsonObject{{"API_KEY", cl->getAPIKEY()}, {"model", cl->getModel()}, {"prompt", prompt}, {"font", font_size}}).toJson());
    file.flush();
    file.close();
}

void AIWidget::getChat(const QString &path)
{
    QFile file(path);

    file.open(QIODevice::ReadOnly);

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject doc_object = doc.object();

    QString current_chat = doc_object["current"].toString();
    QStringList chats = doc_object["history"].toObject().keys();

    emit sendChat(current_chat, chats);
}
