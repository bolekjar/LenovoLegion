// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "ProtocolProcessorBase.h"

#include <Core/LoggerHolder.h>

#include <QCoreApplication>

namespace LenovoLegionDaemon {

ProtocolProcessorBase::ProtocolProcessorBase(QLocalSocket* clientSocket,QObject* parent) :
    QObject(parent),
    m_clientSocket(clientSocket)
{
    if(m_clientSocket == nullptr)
    {
        THROW_EXCEPTION(exception_T,CLIENT_POINTER_ERROR,"Client socket is nullptr !");

    }
}

ProtocolProcessorBase::~ProtocolProcessorBase()
{
    m_clientSocket->close();
    waitForExit();
    m_clientSocket->deleteLater();

    disconnect(m_clientSocket, &QLocalSocket::readyRead,this,&ProtocolProcessorBase::on_readyReadHandler);
    disconnect(m_clientSocket, &QLocalSocket::disconnected,this, &ProtocolProcessorBase::on_disconnectedHandler);

    LOG_D("ProtocolProcessorBase destroyed !");
}

void ProtocolProcessorBase::stop()
{
    LOG_D("ProtocolProcessorBase stopped !");

    m_clientSocket->close();
}

void ProtocolProcessorBase::start()
{
    LOG_D("ProtocolProcessorBase started !");

    connect(m_clientSocket, &QLocalSocket::readyRead,this,&ProtocolProcessorBase::on_readyReadHandler);
    connect(m_clientSocket, &QLocalSocket::disconnected,this, &ProtocolProcessorBase::on_disconnectedHandler);
}

void ProtocolProcessorBase::waitForExit()
{
    while (isRunning())
    {
        QCoreApplication::processEvents();
    }
}

bool ProtocolProcessorBase::isRunning() const
{
    return  m_clientSocket->isOpen();
}

void ProtocolProcessorBase::on_readyReadHandler()
{
    readyReadHandler();
}

void ProtocolProcessorBase::on_disconnectedHandler()
{
    LOG_D("Client disconnected !");
    m_clientSocket->close();
}



void ProtocolProcessorBase::refuseConnection(QLocalSocket *clientSocket)
{
    if(clientSocket == nullptr)
    {
        THROW_EXCEPTION(exception_T,CLIENT_POINTER_ERROR,"Client socket is nullptr !");

    }

    clientSocket->close();
    clientSocket->deleteLater();
}

}
