// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "PowerProfileControl.h"
#include "ui_PowerProfileControl.h"


#include <PowerProfileControlDataProvider.h>


namespace LenovoLegionGui {

PowerProfileControl::PowerProfileControl(PowerProfileControlDataProvider *dataProvider, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PowerProfileControl)
    , m_dataProvider(dataProvider)
{
    ui->setupUi(this);

    /* Read Data */
    m_powerProfileControlData = m_dataProvider->getPowerProfileData();
    m_batteryControlData      = m_dataProvider->getBateryControlData();

    renderData();
}

PowerProfileControl::~PowerProfileControl()
{
    delete ui;
}

void PowerProfileControl::refresh()
{
    m_powerProfileControlData = m_dataProvider->getPowerProfileData();
    m_batteryControlData      = m_dataProvider->getBateryControlData();

    renderData();
}

void PowerProfileControl::on_radioButton_PPPerformance_clicked()
{
    m_dataProvider->setPowerProfileData(LenovoLegionDaemon::PowerProfile::Control::DataControl {LenovoLegionDaemon::PowerProfile::Control::PowerProfiles::POWER_PROFILE_PERFORMANCE});
    m_powerProfileControlData = m_dataProvider->getPowerProfileData();
    renderData();
}

void PowerProfileControl::on_radioButton_PPCustom_clicked()
{
    m_dataProvider->setPowerProfileData(LenovoLegionDaemon::PowerProfile::Control::DataControl {LenovoLegionDaemon::PowerProfile::Control::PowerProfiles::POWER_PROFILE_CUSTOM});
    m_powerProfileControlData = m_dataProvider->getPowerProfileData();
    renderData();
}

void PowerProfileControl::on_radioButton_PPBalanced_clicked()
{
    m_dataProvider->setPowerProfileData(LenovoLegionDaemon::PowerProfile::Control::DataControl {LenovoLegionDaemon::PowerProfile::Control::PowerProfiles::POWER_PROFILE_BALANCED});
    m_powerProfileControlData = m_dataProvider->getPowerProfileData();
    renderData();
}

void PowerProfileControl::on_radioButton_PPQuiet_clicked()
{
    m_dataProvider->setPowerProfileData(LenovoLegionDaemon::PowerProfile::Control::DataControl {LenovoLegionDaemon::PowerProfile::Control::PowerProfiles::POWER_PROFILE_QUIET});
    m_powerProfileControlData = m_dataProvider->getPowerProfileData();
    renderData();
}

void PowerProfileControl::renderData()
{
    if(!m_powerProfileControlData.m_isAvailable)
    {
        emit widgetEvent( LenovoLegionGui::WidgetMessage {
            .m_widget       = LenovoLegionGui::WidgetMessage::Widget::POWER_PROFILE_CONTROL,
            .m_message = LenovoLegionGui::WidgetMessage::Message::POWER_PROFILE_NOT_AVAILABLE
        });

        return;
    }

    switch (m_powerProfileControlData.m_data.m_profile) {
    case LenovoLegionDaemon::PowerProfile::Control::PowerProfiles::POWER_PROFILE_QUIET:
        ui->radioButton_PPQuiet->setChecked(true);
        break;
    case LenovoLegionDaemon::PowerProfile::Control::PowerProfiles::POWER_PROFILE_BALANCED:
        ui->radioButton_PPBalanced->setChecked(true);
        break;
    case LenovoLegionDaemon::PowerProfile::Control::PowerProfiles::POWER_PROFILE_PERFORMANCE:
        ui->radioButton_PPPerformance->setChecked(true);
        break;
    case LenovoLegionDaemon::PowerProfile::Control::PowerProfiles::POWER_PROFILE_CUSTOM:
        ui->radioButton_PPCustom->setChecked(true);
        break;
    default:
        break;
    }

    if(m_batteryControlData.m_data.m_powerChargeMode == LenovoLegionDaemon::Batery::PowerChargeMode::POWER_CHARGE_MODE_AC)
    {
        ui->radioButton_PPCustom->setVisible(true);
        ui->radioButton_PPPerformance->setVisible(true);
    }
    else
    {
        ui->radioButton_PPCustom->setVisible(false);
        ui->radioButton_PPPerformance->setVisible(false);
    }


    emit widgetEvent( LenovoLegionGui::WidgetMessage {
        .m_widget       = LenovoLegionGui::WidgetMessage::Widget::POWER_PROFILE_CONTROL,
        .m_message = m_powerProfileControlData.m_data.m_profile == LenovoLegionDaemon::PowerProfile::Control::PowerProfiles::POWER_PROFILE_CUSTOM  ? LenovoLegionGui::WidgetMessage::Message::POWER_PROFILE_CHANGED_CUSTOM :LenovoLegionGui::WidgetMessage::Message::POWER_PROFILE_CHANGED
    });
}

}
