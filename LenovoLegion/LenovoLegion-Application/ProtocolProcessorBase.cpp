// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "ProtocolProcessorBase.h"

#include <ProtocolParser.h>

#include <Core/LoggerHolder.h>

#include <../LenovoLegion-Daemon/Application.h>

namespace LenovoLegionGui {

ProtocolProcessorBase::ProtocolProcessorBase(const QString& socketName,QObject *parent)
    : QObject(parent)
    , SOCKET_NAME{socketName}
    , m_socket{new QLocalSocket{this}}
{
    connect(m_socket, &QLocalSocket::connected, this, &ProtocolProcessorBase::onConnected);
    connect(m_socket, &QLocalSocket::disconnected, this, &ProtocolProcessorBase::onDisconnected);

    m_timerId = startTimer(1000);
}

ProtocolProcessorBase::~ProtocolProcessorBase()
{
    m_socket->close();
    waitForExit();
}

void ProtocolProcessorBase::timerEvent(QTimerEvent *timerId)
{
    if(timerId->timerId() == m_timerId)
    {
        if(!m_socket->isOpen())
        {
            LOG_D(QString("Connecting to daemon socket ( ").append(SOCKET_NAME).append(" )"));

            m_socket->connectToServer(SOCKET_NAME);
        }
    }
}

void ProtocolProcessorBase::sendMessage(const LenovoLegionDaemon::Message &message)
{
    m_socket->write(LenovoLegionDaemon::ProtocolParser::parseMessage(message));
}

LenovoLegionDaemon::Message ProtocolProcessorBase::receiveMessage(int timeout)
{
    if(m_socket->waitForReadyRead(timeout))
    {
        LenovoLegionDaemon::Message message;
        LenovoLegionDaemon::ProtocolParser::parseMessage(*m_socket,[&message](const LenovoLegionDaemon::Message &msg){
            message = msg;
        });

        return message;
    }

    THROW_EXCEPTION(exception_T, ERROR_CODES::TIMEOUT_ERROR,"Timeout while waiting for message");
}

LenovoLegionDaemon::Message ProtocolProcessorBase::receiveMessageDataReady()
{
    LenovoLegionDaemon::Message message;
    LenovoLegionDaemon::ProtocolParser::parseMessage(*m_socket,[&message](const LenovoLegionDaemon::Message &msg){
        message = msg;
    });

    return message;
}

void ProtocolProcessorBase::onConnected()
{

    LOG_D(QString("Connected to daemon socket ( ").append(SOCKET_NAME).append(" )"));
    emit connected();
}

void ProtocolProcessorBase::onDisconnected()
{
    LOG_D(QString("Disconnected from daemon socket ( ").append(SOCKET_NAME).append(" )"));
    emit disconnected();

    m_socket->close();
}

void ProtocolProcessorBase::waitForExit()
{
    while (isConnected())
    {
        QCoreApplication::processEvents();
    }
}

bool ProtocolProcessorBase::isConnected()
{
    return m_socket->isOpen();
}

}
