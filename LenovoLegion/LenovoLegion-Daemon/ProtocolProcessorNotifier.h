// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include "SysFsDriver.h"
#include "SysFsDriverManager.h"

#include "ProtocolProcessorBase.h"
#include "ControlDataProvider.h"


#include <QLocalSocket>


namespace LenovoLegionDaemon {

class ProtocolProcessorNotifier : public ProtocolProcessorBase
{
    Q_OBJECT

public:

    enum ERROR_CODES : int {
        UNEXPECTED_MESSAGE = -1,
        WATCHED_PATH_ERROR = -2,
    };


public:

    ProtocolProcessorNotifier(ControlDataProvider* dataProvider,QLocalSocket* clientSocket,QObject* parent = nullptr);
    ~ProtocolProcessorNotifier();

    virtual void stop()  override;
    virtual void start() override;

private:

    virtual void readyReadHandler() override;

public slots:

    void kernelEventHandler(const LenovoLegionDaemon::SysFsDriver::SubsystemEvent& event);
    void moduleSubsystemHandler(const LenovoLegionDaemon::SysFsDriverManager::ModuleSubsystemEvent& event);

private:

    ControlDataProvider*     m_controlDataProvider;
};

}

