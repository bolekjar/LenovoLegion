// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#pragma once

#include "RGBControllerInterface.h"

#include <thread>
#include <mutex>
#include <atomic>


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
    void                    SetupColors();

    unsigned int            GetLEDsInZone(unsigned int zone);
    std::string             GetName();
    std::string             GetVendor();
    std::string             GetDescription();
    std::string             GetVersion();
    std::string             GetSerial();
    std::string             GetLocation();

    std::string             GetModeName(unsigned int mode);
    std::string             GetZoneName(unsigned int zone);
    std::string             GetLEDName(unsigned int led);

    RGBColor                GetLED(unsigned int led);
    void                    SetLED(unsigned int led, RGBColor color);
    void                    SetAllLEDs(RGBColor color);
    void                    SetAllZoneLEDs(int zone, RGBColor color);

    int                     GetMode();
    void                    SetMode(int mode);

    void                    RegisterUpdateCallback(RGBControllerCallback new_callback, void * new_callback_arg);
    void                    UnregisterUpdateCallback(void * callback_arg);
    void                    ClearCallbacks();
    void                    SignalUpdate();

    void                    UpdateLEDs();

    void                    UpdateMode();
    void                    SaveMode();

    void                    DeviceCallThreadFunction();

    void                    ClearSegments(int zone);
    void                    AddSegment(int zone, segment new_segment);


    void                    SetDeviceProfile(size_t profile);

    bool                    IsApplayingSettingsInProgress() const;


    /*---------------------------------------------------------*\
    | Functions to be implemented in device implementation      |
    \*---------------------------------------------------------*/
    virtual void            SetupZones()                                = 0;

    virtual void            ResizeZone(int zone, int new_size)          = 0;

    virtual void            DeviceUpdateLEDs()                          = 0;
    virtual void            UpdateZoneLEDs(int zone)                    = 0;
    virtual void            UpdateSingleLED(int led)                    = 0;

    virtual void            DeviceUpdateMode()                          = 0;
    void                    DeviceSaveMode();

    void                    SetCustomMode();



    virtual unsigned int GetFlags() const;
    virtual void SetFlags(unsigned int new_flags);

    virtual const std::vector<led>&       GetLEDs() const;
    virtual void  SetLEDs(const std::vector<led>& new_leds);

    virtual const std::vector<zone>&      GetZones() const;

    virtual std::string                   GetControllerName() const;

    virtual int                           GetActiveMode() const ;

    virtual const std::vector<mode>&      GetModes() const;
    virtual void                          SetModes(const std::vector<mode>& new_modes);

    virtual const std::vector<RGBColor>&  GetColors() const;

    virtual device_type            GetDeviceType() const;

    virtual unsigned int           GetProfiles() const;

    virtual size_t                 GetActiveProfile() const;

protected:

    unsigned int            flags;              /* controller flags         */
    std::vector<led>        leds;               /* LEDs                     */
    std::vector<zone>       zones;              /* Zones                    */
    std::string             name;               /* controller name          */
    int                     active_mode = 0;    /* active mode              */
    std::vector<mode>       modes;              /* Modes                    */
    std::vector<RGBColor>   colors;             /* Color buffer             */
    device_type             type;               /* device type              */
    unsigned int            profiles = 0;       /* Supported Device Profiles*/
    size_t                  active_profile = 0; /* active profile        */


    std::string             vendor;             /* controller vendor        */
    std::string             description;        /* controller description   */
    std::string             version;            /* controller version       */
    std::string             serial;             /* controller serial number */
    std::string             location;           /* controller location      */
    std::vector<std::string>
        led_alt_names;                          /* alternate LED names      */
private:
    std::thread*            DeviceCallThread;
    std::atomic<bool>       CallFlag_UpdateLEDs;
    std::atomic<bool>       CallFlag_UpdateMode;
    std::atomic<bool>       DeviceThreadRunning;

    std::mutex                          UpdateMutex;
    std::vector<RGBControllerCallback>  UpdateCallbacks;
    std::vector<void *>                 UpdateCallbackArgs;
};

}
