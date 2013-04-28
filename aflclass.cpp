#include "aflclass.h"
#include <feesamcli.h>
#include <mainwindow.h>

//TODO: Decide if writereaddata 5 times to get a list of active fecs at startup is necessary.


AFLClass::AFLClass() //Constructor
{ }

bool GetFecActive()
{
    //bool res = MainWindow::_FeeClient->writeReadData();
}

bool SetFecActive(int module, std::string & FeeServerName)
{
   // bool res = MainWindow::_FeeClient->writeReadData(FeeServerName, );
}


// int ret = writeReadData ( serverName, size, data, flags, errorCode, status );
// bool writeReadData(std::string& serverName, size_t& size, DataArray& data, FlagBits& flags, short& errorCode, short& status);

