// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include <Core/ExceptionBuilder.h>

#include <Message.h>

#include <QObject>
#include <QLocalSocket>

namespace LenovoLegionGui {


class ProtocolProcessorBase : public QObject
{
    Q_OBJECT


public:

    enum ERROR_CODES : int {
        TIMEOUT_ERROR     = -1
    };


    DEFINE_EXCEPTION(ProtocolProcessorBase);

public:

    explicit ProtocolProcessorBase(const QString& socketName,QObject *parent = nullptr);
    ~ProtocolProcessorBase();



protected:


    /*
     * Send message
     */
    void sendMessage(const LenovoLegionDaemon::Message &message);

    /*
     * Receive message
     */
    LenovoLegionDaemon::Message receiveMessage(int timeout = 5000);
    LenovoLegionDaemon::Message receiveMessageDataReady();

signals:

    void connected();
    void disconnected();

protected:

    void timerEvent(QTimerEvent *) override;

private:

    void waitForExit();
    bool isConnected();

private slots:

    void onConnected();
    void onDisconnected();
public:

    const QString  SOCKET_NAME;

protected:

    QLocalSocket* m_socket;

    int m_timerId;
};


}
