#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "feeclient/util/feeserver/rcu_issue.h"

#include <QInputDialog>
#include <QVector>
#include <QDialog>
#include "phosdialog.h"
#include <QLabel>


#include "QDebugStream.h" //for redirecting cout to QTextEdit

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
    if(rFSN=true)
    {
        QMessageBox::information(this,"Success","FeeClient registered successfully. \n): Not already registered");
    }
    if(rFSN=false)
    {
        QMessageBox::information(this,"Failed","FeeClient did not register. \n): The FeeClient is already connected or \n serverName is NULL.");
    }



    //Register Service Name - trengs dette? .. fikse senere.. ingen håndterings funksjon i feesamcli..
    //bool rSN = client->registerServiceName(DCS_Name.c_str(),**peker ); //**peker til håndteringsfunksjonenfor tjenesten


    //starting FeeClient
    //int state;
    int state = _FeeClient->startFeeClient();
    if( state == -1 )
    {
        QMessageBox::information(this,"Error","Error when starting FeeClient. \n FeeServer is in a wrong state, \nalready active?");
    }
    else
    {
        QMessageBox::information(this,"FeeClient registered","The FeeClient registered. Number of running services: \n%d",state); //TODO : show state in another way!! check arguments of QMessageBoc::information!!
    }

}

void MainWindow::on_pushButton_2_clicked() //"Dialog"
{

    // how to show a dialog using phosdialog.
    // Output is in this example the output.
    int base_number = 16; //Base of the string which is expected. Change 16 to 2 for binary.
    bool ok;
    QLabel dLabel; //see class reference : QLabel is used for desplaying stuff.
    PHOSDialog *pDialog = new PHOSDialog(&dLabel); //declaring pDialog as a PHOSDialog.
    dLabel.show(); // Not needed. Sets the position of the dialog to the center of the QMainWindow.
    uint Output;
    if (pDialog->exec()) {
    Output = pDialog->InputVal().toUInt(&ok, base_number); //Output is the output of the dialog.
    }
    if(ok = false) //if fail
    {
        QMessageBox::information(this, "Conversion error!","Could not convert the input to uint (uint for reference).");
        return;
    }
    if(ok = true)
    {
    //For debigging purposes
    QString string;
    string.setNum(Output,base_number);
    QMessageBox::information(this,"New AFL is:", string);
    return;
    }
}

void MainWindow::on_pushButton_3_clicked()//Read AFL
{
    //Start timer here

    //from readRegisters.cpp
    Register* AFL = new ACTFECLIST(0x0);    //Create a new ACTFECLIST register with no active FECs

    int result=0;
    if(_FeeClient!=NULL)                    //Check if FeeSampleClient implementation exists
        result=_FeeClient->readAFL(AFL);    //Attempt to read AFL register

        if (result == 1)    //Success
        {
            //Stop timer here

            //vector<uint> actfeclist = _FeeClient->

            QMessageBox::information(this,"Success!",QString::number(AFL->GetValue())); //Display list of active FECs
        }

        else
            QMessageBox::information(this,"Failure!","Something went wrong");
}

void MainWindow::on_pushButton_4_clicked() //PushButton4
{

}

void MainWindow::on_pushButton_5_clicked() //PushButton5
{

}
