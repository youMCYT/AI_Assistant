#include "aiwidget.h"
#include "ui_aiwidget.h"

AIWidget::AIWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AIWidget)
{
    ui->setupUi(this);
}

AIWidget::~AIWidget()
{
    delete ui;
}
