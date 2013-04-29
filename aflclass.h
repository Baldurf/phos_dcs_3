#ifndef AFLCLASS_H
#define AFLCLASS_H

#ifndef __VECTOR__
# include <vector>
#endif
#ifndef __STRING__
# include <string>
#endif
#include <feesamcli.h>
#include <mainwindow.h>

class AFLClass
{
public:
    AFLClass();

private:
    bool GetFecActive(FeeSamCli _feeServer);
    bool SetFecActive(int module, int rcu, std::string & FeeServerName );

};

#endif // AFLCLASS_H
