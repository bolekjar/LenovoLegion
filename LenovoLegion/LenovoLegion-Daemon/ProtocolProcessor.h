// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include "ControlDataProvider.h"
#include "ProtocolProcessorBase.h"

#include <Core/ExceptionBuilder.h>

#include <QObject>
#include <QLocalSocket>
#include <QFileSystemWatcher>



namespace LenovoLegionDaemon {

class ProtocolProcessor : public ProtocolProcessorBase
{
    Q_OBJECT

public:

    ProtocolProcessor(ControlDataProvider* dataProvider, QLocalSocket* clientSocket,QObject* parent = nullptr);
    ~ProtocolProcessor();

    virtual void stop() override;
    virtual void start() override;

private slots:

    virtual void readyReadHandler() override;

private:

    ControlDataProvider*     m_controlDataProvider;
};

}
