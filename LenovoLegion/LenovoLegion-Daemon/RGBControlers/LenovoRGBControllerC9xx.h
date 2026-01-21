// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#pragma once

#include "LenovoRGBController.h"
#include "LenovoUSBControllerC9xx.h"


namespace LenovoLegionDaemon {

class LenovoRGBControllerC9xx : public LenovoRGBController
{

public:

    LenovoRGBControllerC9xx(LenovoUSBControllerC9xx* controller_ptr);


    void    DeviceUpdateEfects()                   override;
    void    DeviceRefreshEffects()                 override;

protected:

    void timerEvent(QTimerEvent* event) override;

private:
    void startStopDirectControlModeTimerIfNeeded();

private:

    int  m_timerId;
};

}
