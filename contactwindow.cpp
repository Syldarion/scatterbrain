#include "contactwindow.h"
#include "ui_contactwindow.h"

ContactWindow::ContactWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactWindow)
{
    ui->setupUi(this);
}

ContactWindow::~ContactWindow()
{
    delete ui;
}
