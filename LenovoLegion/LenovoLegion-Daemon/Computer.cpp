#include "Computer.h"

#include "SysFSDriverLegionRaplMMIO.h"
#include "SysFsDriverManager.h"


#include "SysFsDataProvider.h"


#include <Core/LoggerHolder.h>



namespace LenovoLegionDaemon {

Computer::Computer(SysFsDriverManager& sysFsDriverManager,QObject *parent) : QObject(parent), m_sysFsDriverManager(sysFsDriverManager)
{}

void Computer::initComputer()
{
    LOG_I("Initializing Computer");

    try {
        SysFSDriverLegionRaplMMIO::RaplMMIO raplMMIO(m_sysFsDriverManager.getDriverDesriptor(SysFSDriverLegionRaplMMIO::DRIVER_NAME));

        LOG_I("\tChecking Rapl MMIO registers:");

        if(raplMMIO.m_is_locked.empty()                 ||
           raplMMIO.m_is_pl4_locked.empty()             ||
           raplMMIO.m_pl1_current_value.empty()         ||
           raplMMIO.m_pl2_current_value.empty()         ||
           raplMMIO.m_pl4_current_value.empty()         ||
           raplMMIO.m_set_and_lock.empty()              ||
           raplMMIO.m_set_pl4_and_lock.empty()
            )
        {
            LOG_E("\t\tSome Rapl MMIO registers are missing !");

            THROW_EXCEPTION(exception_T,ERROR_CODES::INITIALIZATION_ERROR,"Some Rapl MMIO registers are missing !");
        }

        bool isLocked = SysFsDataProvider::getData(raplMMIO.m_is_locked).toUShort() != 0;
        bool isPl4Locked = SysFsDataProvider::getData(raplMMIO.m_is_pl4_locked).toUShort() != 0;


        quint32 pl1CurrentValue = SysFsDataProvider::getData(raplMMIO.m_pl1_current_value).toUInt();
        quint32 pl2CurrentValue = SysFsDataProvider::getData(raplMMIO.m_pl2_current_value).toUInt();
        quint32 pl4CurrentValue = SysFsDataProvider::getData(raplMMIO.m_pl4_current_value).toUInt();


        if(isLocked && (pl1CurrentValue == LOCKING_VALUE_IN_MW) && (pl2CurrentValue == LOCKING_VALUE_IN_MW))
        {
            LOG_I("\t\tRapl MMIO pl1,pl2 already locked.");
        }
        else
        {
            if(isLocked)
            {
                LOG_E(QString("\t\tSome Rapl MMIO registers already locked with unexpected values(").append(QString::number(pl1CurrentValue)).append(",").append(QString::number(pl2CurrentValue)).append("), restart computer to unlock !"));

                THROW_EXCEPTION(exception_T,ERROR_CODES::INITIALIZATION_ERROR,"Some Rapl MMIO registers already locked with unexpected values!");
            }

            LOG_I("\t\tLocking Rapl MMIO pl1,pl2.");

            SysFsDataProvider::setData(raplMMIO.m_set_and_lock,std::vector<quint32>{LOCKING_VALUE_IN_MW,LOCKING_VALUE_IN_MW});

            isLocked = SysFsDataProvider::getData(raplMMIO.m_is_locked).toUShort() != 0;
            pl1CurrentValue = SysFsDataProvider::getData(raplMMIO.m_pl1_current_value).toUInt();
            pl2CurrentValue = SysFsDataProvider::getData(raplMMIO.m_pl2_current_value).toUInt();


            if(!isLocked || (pl1CurrentValue != LOCKING_VALUE_IN_MW) || (pl2CurrentValue != LOCKING_VALUE_IN_MW))
            {
                LOG_E("\t\tFailed to lock Rapl MMIO pl1,pl2 !");

                THROW_EXCEPTION(exception_T,ERROR_CODES::INITIALIZATION_ERROR,"Failed to lock Rapl MMIO pl1,pl2 !");
            }
        }


        if(isPl4Locked && (pl4CurrentValue == LOCKING_VALUE_IN_MW))
        {
            LOG_I("\t\tRapl MMIO pl4 already locked.");
        }
        else
        {
            if(pl4CurrentValue == 0)
            {
                LOG_I("\t\tRapl MMIO pl4 is not supported skipping locking  !");
            }
            else
            {
                LOG_I("\t\tLocking Rapl MMIO pl4.");

                SysFsDataProvider::setData(raplMMIO.m_set_pl4_and_lock,LOCKING_VALUE_IN_MW);

                isPl4Locked = SysFsDataProvider::getData(raplMMIO.m_is_pl4_locked).toUShort() != 0;
                pl4CurrentValue = SysFsDataProvider::getData(raplMMIO.m_pl4_current_value).toUInt();

                if(!isPl4Locked || (pl4CurrentValue != LOCKING_VALUE_IN_MW))
                {
                    LOG_E("\t\tFailed to lock Rapl MMIO pl4 !");

                    THROW_EXCEPTION(exception_T,ERROR_CODES::INITIALIZATION_ERROR,"Failed to lock Rapl MMIO pl4 !");
                }
            }
        }
    } catch (SysFsDriver::exception_T& ex)
    {
        if(ex.errcodeInfo().value() == SysFsDriver::DRIVER_NOT_AVAILABLE)
        {
            LOG_W("Rapl MMIO SysFs driver not available, skipping Rapl MMIO initialization !");
            return;
        }
        else
        {
            LOG_E("Rapl MMIO SysFs driver error, cannot initialize computer !");
            THROW_EXCEPTION(exception_T,ERROR_CODES::INITIALIZATION_ERROR,"Rapl MMIO SysFs driver error, cannot initialize computer !");
        }
    }

    LOG_I("Computer initialized");
}


}


