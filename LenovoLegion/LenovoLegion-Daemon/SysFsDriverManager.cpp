// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "SysFsDriverManager.h"

#include <Core/LoggerHolder.h>

#include <QSocketNotifier>

#include <SysFsDriver.h>

#include <libudev.h>

namespace LenovoLegionDaemon {

const  SysFsDriver::KernelEvent::Filter SysFsDriverManager::MODULE_SUBSYSTEM_EVENT_FILTER = { "module" };

SysFsDriverManager::SysFsDriverManager(QObject *parent)
    : QObject{parent},
    m_udev(udev_new()),
    m_mon(nullptr),
    m_socketNotifier(nullptr)
{
    if(m_udev == nullptr)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::UDEV_INITIALIZED_ERROR,"Udev new error !");
    }

    m_mon = udev_monitor_new_from_netlink(m_udev, SysFsDriver::KernelEvent::Filter::NAME.data());

    if(m_mon == nullptr)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::UDEV_INITIALIZED_ERROR,"Udev monitor error !");
    }

    if(udev_monitor_enable_receiving(m_mon) < 0)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::UDEV_INITIALIZED_ERROR,"Udev monitor enable error !");
    }

    m_socketNotifier = new QSocketNotifier(udev_monitor_get_fd(m_mon), QSocketNotifier::Read, this);

    connect(m_socketNotifier,&QSocketNotifier::activated,this,&SysFsDriverManager::onDataReceived);

    /*
     * Add module subsystem filter
     */
    addUdevMonitorFilter(MODULE_SUBSYSTEM_EVENT_FILTER);
}

SysFsDriverManager::~SysFsDriverManager()
{
    /* free udev monitor */
    udev_monitor_unref(m_mon);

    /* free udev */
    udev_unref(m_udev);

    cleanDrivers();
}

void SysFsDriverManager::addDriver(SysFsDriver *driver)
{
    if(driver == nullptr)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::DRIVER_NOT_FOUND,"Driver is nullptr !");
    }

    if(m_drivers.insert({driver->m_name,driver}).second == false)
    {
        THROW_EXCEPTION(exception_T,ERROR_CODES::DRIVER_ALREADY_LOADED,"Driver already loaded !");
    };
}

void SysFsDriverManager::initDrivers()
{
    for(auto& driver : m_drivers)
    {
        driver.second->init();
        driver.second->validate();

        connect(driver.second,&SysFsDriver::kernelEvent,this,&SysFsDriverManager::onKernelEvent);

        addUdevMonitorFilter(driver.second->m_filter);
    }
}

void SysFsDriverManager::cleanDrivers()
{
    for(auto& driver : m_drivers)
    {
        driver.second->clean();
    }

    m_drivers.clear();
}

void SysFsDriverManager::blockKernelEvent(const QString &driverName, bool block)
{
    try {
        m_drivers.at(driverName)->blockKernelEvent(block);
    } catch (const std::out_of_range& ex) {
        THROW_EXCEPTION(exception_T,ERROR_CODES::DRIVER_NOT_FOUND,"Driver not found !");
    }
}

void SysFsDriverManager::blockSignals(const QString &driverName,bool block)
{
    try {
        m_drivers.at(driverName)->blockSignals(block);
    } catch (const std::out_of_range& ex) {
        THROW_EXCEPTION(exception_T,ERROR_CODES::DRIVER_NOT_FOUND,"Driver not found !");
    }
}

const SysFsDriver::DescriptorType&  SysFsDriverManager::getDriverDesriptor(const QString &driverName) const
{
    try {
        return m_drivers.at(driverName)->desriptor();
    } catch (const std::out_of_range& ex) {
        THROW_EXCEPTION(exception_T,ERROR_CODES::DRIVER_NOT_FOUND,"Driver not found !");
    }
}

const SysFsDriver::DescriptorsInVectorType &SysFsDriverManager::getDriverDescriptorsInVector(const QString &driverName) const
{
    try {
        return m_drivers.at(driverName)->descriptorsInVector();
    } catch (const std::out_of_range& ex) {
        THROW_EXCEPTION(exception_T,ERROR_CODES::DRIVER_NOT_FOUND,"Driver not found !");
    }
}

void SysFsDriverManager::onDataReceived(int)
{
    struct udev_device *dev = udev_monitor_receive_device(m_mon);

    if(dev != nullptr)
    {
        SysFsDriver::KernelEvent::Event event = {
            .m_action    = udev_device_get_action(dev)   ,
            .m_driver    = udev_device_get_driver(dev)   ,
            .m_sysName   = udev_device_get_sysname(dev)  ,
            .m_subSystem = udev_device_get_subsystem(dev),
            .m_devPath   = udev_device_get_devpath(dev)  ,
        };

        if(event.m_subSystem == MODULE_SUBSYSTEM_EVENT_FILTER.m_subSystem)
        {
            LOG_D("Kernel event received ACTION=" + event.m_action + ", DRIVER=" + event.m_driver + ", SYSNAME=" + event.m_sysName + ", SUBSYSTEM=" + event.m_subSystem + ", DEVPATH=" + event.m_devPath);

            for(auto& driver : m_drivers)
            {
                if(driver.second->m_name == event.m_sysName)
                {
                    if(event.m_action == "add")
                    {
                        driver.second->init();
                        driver.second->validate();

                        emit moduleSubsystem({
                            .m_driverName = driver.second->m_name,
                            .m_action     = ModuleSubsystemEvent::Action::ADD
                        });
                    }

                    if(event.m_action == "remove")
                    {
                        driver.second->clean();

                        emit moduleSubsystem({
                            .m_driverName = driver.second->m_name,
                            .m_action     = event.m_action == "add" ? ModuleSubsystemEvent::Action::ADD : ModuleSubsystemEvent::Action::REMOVE
                        });
                    }
                }
            }
        }
        else
        {
            for (auto& driver : m_drivers)
            {
                if(driver.second->m_filter.m_subSystem == event.m_subSystem)
                {
                    driver.second->handleKernelEvent(event);
                }
            }
        }

        udev_device_unref(dev);
    }
}

void SysFsDriverManager::onKernelEvent(const SysFsDriver::SubsystemEvent &event)
{
    emit kernelEvent(event);
}

void SysFsDriverManager::addUdevMonitorFilter(const SysFsDriver::KernelEvent::Filter &filter)
{
    if(filter.m_subSystem.size() > 0)
    {
        if(udev_monitor_filter_add_match_subsystem_devtype(m_mon, filter.m_subSystem.toStdString().c_str(), nullptr) < 0)
        {
            THROW_EXCEPTION(exception_T,ERROR_CODES::UDEV_INITIALIZED_ERROR,"Udev monitor filter add error !");
        }
    }
}

}
