#ifndef FEESAMCLI_H
#define FEESAMCLI_H

#include <feeclient/include/FeeSampleClient.hpp>
#include <QtCore>
#include <QMutex>

#include "registers/rcu/actfeclist.h"
#include "feeclient/util/feeserver/rcu_issue.h"
using namespace std;

class FeeSamCli : public dcs::fee::FeeSampleClient
{
public:
    /** Default constructor */
    FeeSamCli(QString feeServerName);

    /** Destructor */
    virtual ~FeeSamCli();

    bool readAFL(Register *AFL);

    /** FEE server name */
    QString _feeServerName;

private:



    /** Mutex lock */
    QMutex *_mutex;

    /** Default constructor, prohibited */
    FeeSamCli();

    /** Copy Constructor, prohibited */
    FeeSamCli(const FeeSamCli& other);

    /** Assignment operator, prohibited */
    FeeSamCli& operator=(const FeeSamCli& other);

};
#endif // FEESAMCLI_H
