#-------------------------------------------------
#
# Project created by QtCreator 2013-04-16T12:48:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = phos_dcs_3
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    feeclient/util/dcsctools/mrshellprim.c \
    feeclient/dim/src/utilities.c \
    feeclient/dim/src/tokenstring.cxx \
    feeclient/dim/src/tcpip.c \
    feeclient/dim/src/swap.c \
    feeclient/dim/src/sll.c \
    feeclient/dim/src/open_dns.c \
    feeclient/dim/src/hash.c \
    feeclient/dim/src/dtq.c \
    feeclient/dim/src/dna.c \
    feeclient/dim/src/dll.c \
    feeclient/dim/src/discpp.cxx \
    feeclient/dim/src/dis.c \
    feeclient/dim/src/dim_thr.c \
    feeclient/dim/src/dimcpp.cxx \
    feeclient/dim/src/diccpp.cxx \
    feeclient/dim/src/dic.c \
    feeclient/dim/src/copy_swap.c \
    feeclient/dim/src/conn_handler.c \
    feeclient/src/LockGuard.cpp \
    feeclient/src/FeeSampleClient.cpp \
    feeclient/src/FeePacket.cpp \
    feeclient/src/FeeMsgInfo.cpp \
    feeclient/src/FeeItemInfo.cpp \
    feeclient/src/FeeInfo.cpp \
    feeclient/src/FeeClientLogger.cpp \
    feeclient/src/FeeClientLibInterface.cpp \
    feeclient/src/FeeClientCore.cpp \
    feeclient/src/FeeAckInfo.cpp \
    feeclient/src/AckData.cpp \
    feesamcli.cpp \
    aflclass.cpp \
    phosdialog.cpp \
    registers/register.cpp \
    registers/rcu/actfeclist.cpp

HEADERS  += mainwindow.h \
    feeclient/dim/dim/tokenstring.hxx \
    feeclient/dim/dim/sllist.hxx \
    feeclient/dim/dim/dllist.hxx \
    feeclient/dim/dim/dis.hxx \
    feeclient/dim/dim/dis.h \
    feeclient/dim/dim/dim_core.hxx \
    feeclient/dim/dim/dim_common.h \
    feeclient/dim/dim/dim.hxx \
    feeclient/dim/dim/dim.h \
    feeclient/dim/dim/dic.hxx \
    feeclient/dim/dim/dic.h \
    feeclient/include/Timestamp.hpp \
    feeclient/include/MessageStruct.hpp \
    feeclient/include/LockGuard.hpp \
    feeclient/include/FeeServiceStruct.hpp \
    feeclient/include/FeeSampleClient.hpp \
    feeclient/include/feepacket_flags.h \
    feeclient/include/FeePacket.hpp \
    feeclient/include/FeeMsgInfo.hpp \
    feeclient/include/fee_loglevels.h \
    feeclient/include/FeeItemInfo.hpp \
    feeclient/include/FeeInfo.hpp \
    feeclient/include/fee_errors.h \
    feeclient/include/FeeClientLogger.hpp \
    feeclient/include/FeeClientLibInterface.hpp \
    feeclient/include/FeeClientCore.hpp \
    feeclient/include/FeeAckInfo.hpp \
    feeclient/include/AckData.hpp \
    feeclient/src/FeeClientLinkDef.h \
    feeclient/util/dcsctools/mrshellprim.h \
    feeclient/util/feeserver/rcu_issue.h \
    feesamcli.h \
    aflclass.h \
    QDebugStream.h \
    phosdialog.h \
    registers/register.h \
    registers/rcu/actfeclist.h

FORMS    += mainwindow.ui

OTHER_FILES +=
