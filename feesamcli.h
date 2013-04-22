#ifndef FEESAMCLI_H
#define FEESAMCLI_H

#include <feeclient/include/FeeSampleClient.hpp>
#include <QtCore>

class FeeSamCli : public dcs::fee::FeeSampleClient
{
public:
    FeeSamCli();

    /** Default constructor */
    FeeSamCli(QString feeServerName);

private:
    /** FEE server name */
    QString _feeServerName;

};
#endif // FEESAMCLI_H
