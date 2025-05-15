#include "aboutdialog.h"
#include "ui_aboutdialog.h"

aboutDialog::aboutDialog(QWidget *parent): QDialog(parent), ui(new Ui::aboutDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("About");
    this->setWindowIcon(QIcon("://icon.ico"));
}

aboutDialog::~aboutDialog()
{
    delete ui;
}

void aboutDialog::onAboutDialogAccepted()
{

}
