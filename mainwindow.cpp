#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "feeclient/util/feeserver/rcu_issue.h"

#include <QInputDialog>
#include <QVector>
#include <QDialog>
#include "phosdialog.h"
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //QLabel label;
    //PHOSDialog *pDialog = new PHOSDialog(this);
    //parent.show;
    //FeeSamCli *_FeeClient = new FeeSamCli(); //Why can't this be here?
    this->setWindowTitle ("PHOS_DCS_3"); //changes the name of the mainwindow to PHOS_DCS_3


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked() //"initiate"
{
    //std::string DCS_Name = ui->lineEdit->text().toLatin1().data();
    QString DCS_Name = ui->lineEdit->text();
    _FeeClient = new FeeSamCli(DCS_Name);

    if(ui->checkBox->isChecked())
        QMessageBox::information(this,"DCS_Name",DCS_Name);
    //QMessageBox::information(this,"DCS_Name",DCS_Name.c_str());//ui->lineEdit->text().toLatin1().data();

    const char *_servername = (const char*)DCS_Name.toStdString().c_str();
    //const char *_servername = (const char*)DCS_Name.c_str();
    //Register Server Name
    bool rFSN = _FeeClient->registerFeeServerName(_servername);
    if(rFSN==true)
    {
        QMessageBox::information(this,"Success","FeeClient registered successfully. \nFeeServer Registered");
    }
    if(rFSN==false)
    {
        QMessageBox::information(this,"Failed","FeeClient did not register. \n): The FeeClient is already connected or \n serverName is NULL.");
    }


//Just an empty line

    //Register Service Name - trengs dette? .. fikse senere.. ingen håndterings funksjon i feesamcli..
    //bool rSN = client->registerServiceName(DCS_Name.c_str(),**peker ); //**peker til håndteringsfunksjonenfor tjenesten


    //starting FeeClient
    //int state;
    int state = _FeeClient->startFeeClient();
    if( state == -1 )
    {
        QMessageBox::information(this,"Error",
                                 "Error when starting FeeClient. \n FeeServer is in a wrong state, \nalready active?");
    }
    else
    {
        QString stateStr;
        QMessageBox::information(this,"FeeClient registered",stateStr.append(QString("%1").arg(state)));
    }

}

void MainWindow::on_pushButton_2_clicked() //"Dialog"
{

    QString message = "Give your input";
    PHOSDialog *pDia = new PHOSDialog;
    QString streeng = pDia->sPHOSDialog(message);

    uint talll = pDia->iPHOSDialog(2, message); //change 2 to expected base number.

}

void MainWindow::on_pushButton_3_clicked()//Read AFL
{
    //Start timer here

    //from readRegisters.cpp
    Register* AFL = new ACTFECLIST(0x0);    //Create a new ACTFECLIST register with no active FECs

    bool result = false;
    if(_FeeClient!=NULL)                    //Check if FeeSampleClient implementation exists
        result=_FeeClient->readAFL(AFL);    //Attempt to read AFL register

        if (result == true)    //Success
        {
            //Stop timer here

            //vector<uint> actfeclist = _FeeClient->

            QMessageBox::information(this,"Success!",QString::number(AFL->GetValue())); //Display list of active FECs // QString::number(AFL->GetValue()
        }
        if(result==false)
        {
            QMessageBox::information(this,"readAFL failed","AFL was not read.\nSomething is wrong!");
        }

        else
            QMessageBox::information(this,"Failure!","Something went wrong");
}

void MainWindow::on_pushButton_4_clicked() //stop
{
    if(_FeeClient==NULL)
        return;
    {
    bool resu = _FeeClient->stopFeeClient();
    if (resu==false)
    {
        QMessageBox::information(this,"Failure!","FeeClient was not running");
        return;
    }

    if (resu == true)
    {
        QMessageBox::information(this,"Win!","FeeClient was running.\nShutting down now.");
        delete _FeeClient;
    }
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    if(_FeeClient!=NULL){
    QMessageBox::information(this,"1!","=1");
    return;
    }
    if(_FeeClient==NULL){
    QMessageBox::information(this,"2!","=2");
    return;
    }
}
