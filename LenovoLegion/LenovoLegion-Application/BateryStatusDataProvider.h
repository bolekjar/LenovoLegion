// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include <ControlData.h>

#include <QObject>


namespace  LenovoLegionGui {

class ProtocolProcessor;

class BateryStatusDataProvider : public QObject
{
    Q_OBJECT

public:

    explicit BateryStatusDataProvider(ProtocolProcessor * protocolProcessor,QObject *parent = nullptr);

    LenovoLegionDaemon::Batery::DataInfo   getBateryControlData() const;

signals:

private:
    ProtocolProcessor*  m_protocolProcessor;
};

}
