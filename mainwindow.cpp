#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    char* DCS_Name = ui->lineEdit->text().toLatin1().data();
    if(ui->checkBox->isChecked())
    QMessageBox::information(this,"DCS_Name",DCS_Name);//ui->lineEdit->text()

    _FeeClient = new FeeSamCli();
    FeeSamCli *client = new FeeSamCli();
    client->registerFeeServerName(DCS_Name);
    client->startFeeClient();
}
