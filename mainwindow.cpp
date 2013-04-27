#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "feeclient/util/feeserver/rcu_issue.h"

#include <aflclass.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    FeeSamCli *_FeeClient = new FeeSamCli(); //TODO er dette dust?
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked() //"init"
{
    std::string DCS_Name = ui->lineEdit->text().toLatin1().data();
    int state;

    if(ui->checkBox->isChecked())
    QMessageBox::information(this,"DCS_Name",DCS_Name.c_str());//ui->lineEdit->text()


    //Register Server Name
    bool rFSN = _FeeClient->registerFeeServerName(DCS_Name.c_str());
    if(rFSN=true)
    {
        QMessageBox::information(this,"Success","FeeClient registered successfully. \n): Not already registered");
    }
    if(rFSN=false)
    {
        QMessageBox::information(this,"Failed","FeeClient did not register. \n): The FeeClient is already connected or \n serverName is NULL.");
    }


    /*
    //Register Service Name - trengs dette? .. fikse senere .. ingen håndterings sunksjon i fee.sam.cli..
    bool rSN = client->registerServiceName(DCS_Name.c_str(),**peker ); **peker til håndteringsfunksjonenfor tjenesten
    */

    //starting FeeClient
    state = _FeeClient->startFeeClient();
    if( state == -1 )
    {
        QMessageBox::information(this,"Error","Error when starting FeeClient. \n FeeServer is in a wrong state, \nalready active?");
    }
    else
    {
        QMessageBox::information(this,"FeeClient registered","The FeeClient registered. Number of running services: \n%d",state);
    }
}

void MainWindow::on_pushButton_2_clicked() //"activate"
{
    /* TODO fra 23.04.13
    int num = 3; //set to number of 32 bit words to send
    uint BinData[num];
    uint Bheader, Bparameter, Btailer;
    Btailer = CE_CMD_TAILER; //it's liek stupid!
    Bheader = (FEESERVER_CE_CMD |

    uint BinFile = 0xff;

    */

    //AFLClass *AcFeLi = new AFLClass();

    std::vector< unsigned int > fec0;





    //std::cout << "send this to Text Edit!" << endl;
}

void MainWindow::on_pushButton_3_clicked()//"stop"
{

    /*//exec
    //enum mentos = fsc->ProcessState;

    if( fsc->state() == QProcess::Running )
    fsc->~QProcess();
    return;
    */
}
