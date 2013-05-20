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
bool FeeSamCli::readAFL(Register* AFL)
{
    //Locking mutex. Will be unlocked when readAFL is exited.
    //Variables in this functions can not be changed by other threads
    QMutexLocker mlAFL(_mutex);

    //Declare the binary file that will be sent to the DCS
    //This file will later contain the result (current AFL)
    vector<uint> binary;

    //Build the binary file that will be sent to the DCS
    binary.push_back (RCU_READ_MEMBLOCK | 1);  //Header, read one word (32bit)
    binary.push_back (AFL->GetAddress());      //Read from register 0x5100 (AFL)
    binary.push_back (CE_CMD_TAILER);          //Tailer, end of file

    //Declare and set parameters needed by writeReadData
    size_t size          = binary.size() *4;    //Number of 8 bit words in binary
    unsigned short flags = 0;                   //Message flags
    short errorCode      = 0;                   //Will be set on return if error occurs
    short status         = 0;                   //Will be set on return

    //Convert DCS name to string
    std::string dcsname =_feeServerName.toStdString();

    //Send binary file to DCS
    //ret will be true (1) on success or false (0) on failure
    bool ret = FeeSamCli::writeReadData ( dcsname, size, binary, flags, errorCode, status );

    //Set Active FEC list value equal to the result from the DCS
    AFL->SetValue(binary[0]);

    //Return return-status
    return ret;
}
