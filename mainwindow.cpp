#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "feeclient/util/feeserver/rcu_issue.h"

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
    int state;

    if(ui->checkBox->isChecked())
    QMessageBox::information(this,"DCS_Name",DCS_Name);//ui->lineEdit->text()

    _FeeClient = new FeeSamCli();
    FeeSamCli *client = new FeeSamCli();
    client->registerFeeServerName(DCS_Name);
    state = client->startFeeClient();
    if( state == -1 );
    QMessageBox::information(this,"Error","Error when starting FeeClient. \n FeeServer is in wrong state, already active?");
}

void MainWindow::on_pushButton_2_clicked()
{
    /* TODO fra 23.04.13
    int num = 3; //set to number of 32 bit words to send
    uint BinData[num];
    uint Bheader, Bparameter, Btailer;
    Btailer = CE_CMD_TAILER; //it's liek stupid!
    //Bheader = (FEESERVER_CE_CMD |

    uint BinFile = 0xff;

    char toSend = ""<< BinFile <<" | /opt/feeclient/bin/feeserver-ctrl --server dcs0193 --ce-command FEESRV_BINARY_PGM";

    QMessageBox::information(this,"toSend:", toSend);

    //fsc->start(toSend);//, QStringList() << "--server" << "dcs0193" << "--ce-command" << "FEESRV_BINARY_PGM");
    bool st = fsc->waitForStarted();
    if( st == false )
        QMessageBox::information(this,"dcs0193 not started", "dcs0193 was not strated! \nError!");
    if( st == true )
        QMessageBox::information(this, "dcs0193 was started" ,"dcs0193 was strated! :D \n!Error!");
    */
}

void MainWindow::on_pushButton_3_clicked()
{

    //enum mentos = fsc->ProcessState;
    if( fsc->state() == QProcess::Running )
    fsc->~QProcess();
    return;

}
