// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#pragma once

#include "HidApiWrapper.h"
#include "DataProvider.h"

#include <vector>
#include <memory>


namespace LenovoLegionDaemon {

#define HID_INTERFACE_ANY -1

class RGBController;
class DataProviderRGBController : public DataProvider
{
    Q_OBJECT

public:

    typedef std::function<RGBController* (hid_device_info *, const std::string &)> HIDDeviceDetectorFunction;
    typedef std::function<RGBController* (hidapi_wrapper wrapper, hid_device_info *, const std::string &)> HIDWrappedDeviceDetectorFunction;

private:

    class BasicHIDBlock
    {
    public:
        std::string name;
        uint16_t vid;
        uint16_t pid;
        int interface;
        int usage_page;
        int usage;

        bool compare(hid_device_info *info);
    };

    class HIDDeviceDetectorBlock : public BasicHIDBlock
    {
    public:
        HIDDeviceDetectorFunction function;
    };

    class HIDWrappedDeviceDetectorBlock : public BasicHIDBlock
    {
    public:
        HIDWrappedDeviceDetectorFunction function;
    };

public:

    enum ERROR_CODES : int {
        INVALID_DATA                        = -1,
        SERIALIZE_ERROR                     = -2
    };

public:

    DataProviderRGBController(QObject* parent);

    virtual ~DataProviderRGBController() override;

    virtual QByteArray serializeAndGetData()                      const override;
    virtual QByteArray deserializeAndSetData(const QByteArray&)         override;


    virtual void init() override;
    virtual void clean() override;

public:

    static void registerControler(std::string name,
                                  HIDDeviceDetectorFunction det,
                                  uint16_t vid,
                                  uint16_t pid,
                                  int interface,
                                  int usage_page,
                                  int usage);

private:

    static std::vector<HIDDeviceDetectorBlock>  hidDeviceDetectorsBlocks;;

private:

    std::unique_ptr<RGBController> m_rgbController;

public:

    static constexpr quint8  dataType = 19;
};

}
