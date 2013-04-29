#include "feesamcli.h"

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

    //phosdcsclient.cpp readrcuregister
    vector<uint> binary;                  //The binary file that will be sent to the DCS

    //binarycompiler.cpp
    binary.push_back (RCU_READ_MEMBLOCK | 1);  //Header, read one word(32bit)

    binary.push_back (AFL->GetAddress());      //Read from register 0x5100

    binary.push_back (CE_CMD_TAILER);          //Tailer

    //phosdcsclient.cpp executebinary
    //std::string DCS_Name = ui->lineEdit->text().toLatin1().data();  //Get DCS name
    //std::string DCS_NAME = "dcs0193";

    size_t size          = binary.size() *4;
    unsigned short flags = 0;
    short errorCode      = 0;
    short status         = 0;

    std::string dcsname = FeeSamCli::_feeServerName.toStdString();

    int ret = FeeSamCli::writeReadData ( dcsname, size, binary, flags, errorCode, status );

    //phosdcsclient.cpp readrcuregister
    AFL->SetValue(binary[0]);

    return ret;
}
