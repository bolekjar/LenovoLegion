// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#pragma once

#include "RGBControllerInterface.h"

namespace LenovoLegionDaemon {

class RGBController : public RGBControllerInterface
{
public:
    /*---------------------------------------------------------*\
    | RGBController base class constructor                      |
    \*---------------------------------------------------------*/
    RGBController();
    virtual ~RGBController();

    /*---------------------------------------------------------*\
    | Generic functions implemented in RGBController.cpp        |
    \*---------------------------------------------------------*/
    std::string             GetName()           const;
    std::string             GetVendor()         const;
    std::string             GetDescription()    const;
    std::string             GetVersion()        const;
    std::string             GetSerial()         const;
    std::string             GetLocation()       const;
    std::string             GetControllerName() const;
    int                     GetActiveMode()     const;

protected:
    void                    SetupColors();

public:

    /*---------------------------------------------------------*\
    | Functions From RGBControllerInterface      |
    \*---------------------------------------------------------*/
    virtual unsigned int                    GetLEDsInZone(unsigned int zone) const   override;
    virtual std::string                     GetModeName(unsigned int mode) const     override;
    virtual std::string                     GetZoneName(unsigned int zone) const     override;
    virtual std::vector<zone>               GetZones() const                         override;
    virtual std::string                     GetLEDName(unsigned int led)     const   override;
    virtual std::vector<RGBColor>           GetColors() const                        override;
    virtual device_type                     GetDeviceType() const                    override;
    virtual unsigned int                    GetProfiles() const                      override;
    virtual size_t                          GetActiveProfile() const                 override;

    virtual RGBColor                GetLED(unsigned int led)        const    override;
    virtual void                    SetLED(unsigned int led, RGBColor color) override;
    virtual void                    SetAllLEDs(RGBColor color)               override;

    virtual int                     GetMode()                          const override;
    virtual void                    SetMode(int mode)                        override;

    virtual std::vector<led>        GetLEDs() const                          override;
    virtual void                    SetLEDs(const std::vector<led>& new_leds)override;

    virtual std::vector<mode>        GetModes() const                               override;
    virtual void                     SetModes(const std::vector<mode>& new_modes)   override;
    virtual void                     SetProfile(size_t profileIdx)                  override;
public:

    /*---------------------------------------------------------*\
    | Functions to be implemented in device implementation      |
    \*---------------------------------------------------------*/
    virtual void        SetupZones()                            =   0;

    virtual void        DeviceUpdateLEDs()                      =   0;
    virtual void        DeviceUpdateMode()                      =   0;

    virtual void        DeviceUpdateProfile()                   =   0;

private:

    void                    SetAllZoneLEDs(int zone, RGBColor color);

protected:

    std::vector<led>        leds;               /* LEDs                     */
    std::vector<zone>       zones;              /* Zones                    */
    std::string             name;               /* controller name          */
    int                     active_mode = 0;    /* active mode              */
    std::vector<mode>       modes;              /* Modes                    */
    std::vector<RGBColor>   colors;             /* Color buffer             */
    device_type             type;               /* device type              */
    unsigned int            profiles = 0;       /* Supported Device Profiles*/
    size_t                  active_profile = 0; /* active profile           */


    std::string             vendor;             /* controller vendor        */
    std::string             description;        /* controller description   */
    std::string             version;            /* controller version       */
    std::string             serial;             /* controller serial number */
    std::string             location;           /* controller location      */
    std::vector<std::string>
        led_alt_names;                          /* alternate LED names      */
};

}
