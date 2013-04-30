#include "feesamcli.h"

#include <QMessageBox>
#include "mainwindow.h"

//Constructor
FeeSamCli::FeeSamCli(QString feeServerName) : FeeSampleClient(),_mutex(new QMutex),_feeServerName(feeServerName)
{
}

//Destructor
FeeSamCli::~FeeSamCli()
{
}

//Function that reads the register containing the list of active FECs
int FeeSamCli::readAFL(Register* AFL)
{

    //from phosdcsclient.cpp readrcuregister
    vector<uint> binary;                  //The binary file that will be sent to the DCS

    //from binarycompiler.cpp
    binary.push_back (RCU_READ_MEMBLOCK | 1);  //Header, read one word(32bit)

    binary.push_back (AFL->GetAddress());      //Read from register 0x5100

    binary.push_back (CE_CMD_TAILER);          //Tailer

    //from phosdcsclient.cpp executebinary
    size_t size          = binary.size() *4;
    unsigned short flags = 0;
    short errorCode      = 0;
    short status         = 0;

    std::string dcsname =_feeServerName.toStdString();  //Convert name to string

    int ret = FeeSamCli::writeReadData ( dcsname, size, binary, flags, errorCode, status ); //Send binary to DCS

    //from phosdcsclient.cpp readrcuregister
    AFL->SetValue(binary[0]);   //Set Active FEC list value equal to the result from the DCS

    return ret; //Return return-status
}
