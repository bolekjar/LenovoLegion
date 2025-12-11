// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "LenovoGen9USBController.h"

#include "StringUtils.h"

#include <iomanip>
#include <sstream>

#include <string.h>



namespace LenovoLegionDaemon {


LenovoGen9USBController::LenovoGen9USBController(hid_device* dev_handle, const char* path, uint16_t in_pid)
{
    dev                     = dev_handle;
    location                = path;
    pid                     = in_pid;

    /*---------------------------------------------------------*\
    | Get device name from HID manufacturer and product strings |
    \*---------------------------------------------------------*/
    wchar_t name_string[HID_MAX_STR];

    hid_get_manufacturer_string(dev, name_string, HID_MAX_STR);
    name = StringUtils::wstring_to_string(name_string);

    hid_get_product_string(dev, name_string, HID_MAX_STR);
    name.append(" ").append(StringUtils::wstring_to_string(name_string));
}

LenovoGen9USBController::~LenovoGen9USBController()
{
    hid_close(dev);
}

uint16_t LenovoGen9USBController::getPid()
{
    return pid;
}

std::string LenovoGen9USBController::getName()
{
    return name;
}

std::string LenovoGen9USBController::getLocation()
{
    return("HID: " + location);
}

void LenovoGen9USBController::setLedsByGroup(uint8_t profile_id, std::vector<led_group> led_groups)
{
    uint8_t buffer[PACKET_SIZE];
    memset(buffer, 0x00, PACKET_SIZE);

    size_t i = 0;
    buffer[i++] = REPORT_ID;
    buffer[i++] = SAVE_PROFILE;
    buffer[i++] = 0xC0;
    buffer[i++] = 0x03;
    buffer[i++] = profile_id;
    buffer[i++] = 0x01;
    buffer[i++] = 0x01;

    for(size_t group = 0; group < led_groups.size() && i < PACKET_SIZE - 21; group++)
    {
        buffer[i++] = (uint8_t)group + 1;  //Group index
        buffer[i++] = 0x06;
        buffer[i++] = 0x01;
        buffer[i++] = led_groups[group].mode;
        buffer[i++] = 0x02;
        buffer[i++] = led_groups[group].speed;
        buffer[i++] = 0x03;
        buffer[i++] = led_groups[group].spin;
        buffer[i++] = 0x04;
        buffer[i++] = led_groups[group].direction;
        buffer[i++] = 0x05;
        buffer[i++] = led_groups[group].color_mode;
        buffer[i++] = 0x06;
        buffer[i++] = 0x00;

        buffer[i++] = (uint8_t)led_groups[group].colors.size();
        for(RGBColor c : led_groups[group].colors)
        {
            buffer[i++] = RGBGetRValue(c);
            buffer[i++] = RGBGetGValue(c);
            buffer[i++] = RGBGetBValue(c);
        }

        std::vector<uint16_t> leds = led_groups[group].leds;
        size_t led_count = std::min(leds.size(), (PACKET_SIZE - i)/2);
        buffer[i++] = (uint8_t)led_count;
        uint8_t* byte_ptr = reinterpret_cast<uint8_t*>(leds.data());
        std::copy(byte_ptr, byte_ptr + led_count * sizeof(uint16_t), buffer + i);
        i+= led_count * sizeof(uint16_t);
    }

    buffer[2] = (uint8_t)i;

    sendFeatureReport(buffer, PACKET_SIZE);
}

void LenovoGen9USBController::setLedsDirectOn(uint8_t profile_id)
{
    uint8_t buffer[PACKET_SIZE];
    memset(buffer, 0x00, PACKET_SIZE);

    size_t i = 0;
    buffer[i++] = REPORT_ID;
    buffer[i++] = SET_DIRECT_MODE;
    buffer[i++] = 0xC0;
    buffer[i++] = 0x03;
    buffer[i++] = 0x01;
    buffer[i++] = profile_id;

    sendFeatureReport(buffer, PACKET_SIZE);
}

void LenovoGen9USBController::setLedsDirectOff(uint8_t profile_id)
{
    uint8_t buffer[PACKET_SIZE];
    memset(buffer, 0x00, PACKET_SIZE);

    size_t i = 0;
    buffer[i++] = REPORT_ID;
    buffer[i++] = SET_DIRECT_MODE;
    buffer[i++] = 0xC0;
    buffer[i++] = 0x03;
    buffer[i++] = 0x02;
    buffer[i++] = profile_id;

    sendFeatureReport(buffer, PACKET_SIZE);
}

void LenovoGen9USBController::setLedsDirect(std::vector<led> &leds, std::vector<RGBColor> &colors)
{
    uint8_t buffer[PACKET_SIZE];
    memset(buffer, 0x00, PACKET_SIZE);

    size_t i = 0;
    buffer[i++] = REPORT_ID;
    buffer[i++] = DIRECT_MODE;
    buffer[i++] = 0xC0;
    buffer[i++] = 0x03;

    for(size_t index = 0; index < leds.size() && i < PACKET_SIZE; index++)
    {
        buffer[i++] = leds[index].value & 0xFF;
        buffer[i++] = leds[index].value >> 8 & 0xFF;
        buffer[i++] = RGBGetRValue(colors[index]);
        buffer[i++] = RGBGetGValue(colors[index]);
        buffer[i++] = RGBGetBValue(colors[index]);
    }

    sendFeatureReport(buffer, PACKET_SIZE);
}

void LenovoGen9USBController::setLedsAllOff(uint8_t profile_id)
{
    uint8_t buffer[PACKET_SIZE] = {REPORT_ID, SAVE_PROFILE, 0xC0, 0x03, profile_id, 0x01, 0x01};

    sendFeatureReport(buffer, PACKET_SIZE);
}

uint8_t LenovoGen9USBController::getCurrentProfileId()
{
    uint8_t buffer[PACKET_SIZE] = {REPORT_ID, GET_ACTIVE_PROFILE, 0xC0, 0x03};

    std::vector<uint8_t> response = getFeatureReport(buffer, PACKET_SIZE);

    return response.size()>4?response[4]:0x01;
}

uint8_t LenovoGen9USBController::getCurrentBrightness()
{
    uint8_t buffer[PACKET_SIZE] = {REPORT_ID, GET_BRIGHTNESS, 0xC0, 0x03};

    std::vector<uint8_t> response = getFeatureReport(buffer, PACKET_SIZE);

    return response.size()>4?response[4]:0x00;
}

std::string LenovoGen9USBController::getFirmwareVersion()
{
    return version;
}

std::string LenovoGen9USBController::getSerialString()
{
    wchar_t serial_string[128];
    int ret = hid_get_serial_number_string(dev, serial_string, 128);

    if(ret != 0)
    {
        return("");
    }

    return(StringUtils::wstring_to_string(serial_string));
}


void LenovoGen9USBController::setBrightness(uint8_t brightness)
{
    uint8_t buffer[PACKET_SIZE] = {REPORT_ID, SET_BRIGHTNESS, 0xC0, 0x03, brightness};

    sendFeatureReport(buffer, PACKET_SIZE);
}

void LenovoGen9USBController::switchProfileTo(uint8_t profile_id)
{
    uint8_t buffer[PACKET_SIZE] = {REPORT_ID, SWITCH_PROFILE, 0xC0, 0x03, profile_id};

    sendFeatureReport(buffer, PACKET_SIZE);
 }

std::vector<LenovoGen9USBController::led_group> LenovoGen9USBController::getProfileSettings(uint8_t profile_id)
{
    uint8_t buffer[PACKET_SIZE] = {REPORT_ID, GET_PROFILE, 0xC0, 0x03, profile_id};

    std::vector<uint8_t> response = getFeatureReport(buffer, PACKET_SIZE);

    std::vector<led_group> groups;

    size_t i = 7;
    while(i < response.size() && response[i] != 0x00){
        i++;

        led_group group {0,0,0,0,0,{},{}};

        /*-----------------*\
        |read group settings|
        \*-----------------*/

        size_t cnt = response[i++];
        for(size_t j = 0; j < cnt && i < response.size(); j++, i+=2)
        {
            switch(response[i])
            {
            case 0x01:
            group.mode = response[i+1];
            break;
            case 0x02:
            group.speed = response[i+1];
            break;
            case 0x03:
            group.spin = response[i+1];
            break;
            case 0x04:
            group.direction = response[i+1];
            break;
            case 0x05:
            group.color_mode = response[i+1];
            break;
            case 0x06:
            //group.mode = response[i+1];
            break;
            }
        }

        /*-----------------*\
        |read group colors  |
        \*-----------------*/

        cnt = response[i++];
        for(size_t j = 0; j < cnt && i < response.size(); j++, i+=3)
        {
            group.colors.push_back(ToRGBColor(response[i],response[i+1],response[i+2]));
        }

        /*-----------------*\
        |read group LEDs    |
        \*-----------------*/

        cnt = response[i++];
        for(size_t j = 0; j < cnt && i < response.size(); j++, i+=2)
        {
            group.leds.push_back(response[i] | response[i+1] << 8);
        }

        groups.push_back(group);
    }

   return groups;

}

void LenovoGen9USBController::sendFeatureReport(uint8_t packet[], size_t packet_size)
{
    hid_send_feature_report(dev, packet, packet_size);
}

std::vector<uint8_t> LenovoGen9USBController::getFeatureReport(uint8_t packet[], size_t packet_size)
{
    sendFeatureReport(packet, packet_size);

    uint8_t read_buffer[PACKET_SIZE] = {REPORT_ID};
    int num_bytes = 0;
    num_bytes = hid_get_feature_report(dev, read_buffer, sizeof(read_buffer));

    std::vector<uint8_t> response = {};
    if(num_bytes > 0)
    {
        response.insert(response.begin(), read_buffer, read_buffer + num_bytes);
    }

    return response;
}

std::string LenovoGen9USBController::ConvertBytesToHex(uint8_t packet[], size_t packet_size)
{
    return ConvertBytesToHex(std::vector<uint8_t>(packet, packet + packet_size));
}

std::string LenovoGen9USBController::ConvertBytesToHex(const std::vector<uint8_t> &input)
{
    std::ostringstream temp_stream;
    for(const uint8_t &oneInputByte : input)
    {
        temp_stream << (temp_stream.tellp()==0 ? "" : " ") << std::setw(2) << std::setfill('0') << std::hex << (int)oneInputByte;
    }
    return temp_stream.str();
}

}
