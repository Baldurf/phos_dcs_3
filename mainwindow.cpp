#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "feeclient/util/feeserver/rcu_issue.h"

#include <aflclass.h>
#include <QInputDialog>
#include <QVector>
#include <QDialog>
#include "phosdialog.h"

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
    FeeSamCli *_FeeClient = new FeeSamCli(DCS_Name);

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
    //QVector< uint_t > AFLt;
    //AFLt = QInputDialog::getInt(this)

    /* TODO fra 23.04.13
    int num = 3; //set to number of 32 bit words to send
    uint BinData[num];
    uint Bheader, Bparameter, Btailer;
    Btailer = CE_CMD_TAILER; //it's liek stupid!
    Bheader = (FEESERVER_CE_CMD |

    uint BinFile = 0xff;

    */

    //AFLClass *AcFeLi = new AFLClass();


    // how to show a dialog using phosdialog
    int base_number = 16; //change 16 to 2 for binary
    bool ok;
    QLabel dLabel; //see class reference
    PHOSDialog *pDialog = new PHOSDialog(&dLabel); //declaring pDialog as a PHOSDialog
    dLabel.show(); // Not needed. Sets the position of the dialog to the center of the QMainWindow
    uint SendData;
    if (pDialog->exec()) {
    SendData = pDialog->InputVal().toUInt(&ok, base_number);
    }
    if(ok = false) //if fail
    {
        QMessageBox::information(this, "Conversion error!","Could not convert the input to uint (uint for reference)");
        return;
    }
    if(ok = true)
    {
    //For debigging purposes
    QString string;
    string.setNum(SendData,base_number);
    QMessageBox::information(this,"New AFL is:", string);
    return;
    }
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
