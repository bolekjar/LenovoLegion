// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include "WidgetMessage.h"

#include <DataProvider.h>

#include <QWidget>

namespace Ui {
class PowerProfileControl;
}

namespace LenovoLegionGui {

class PowerProfileControlDataProvider;

class PowerProfileControl : public QWidget
{
    Q_OBJECT

public:
    explicit PowerProfileControl(PowerProfileControlDataProvider * dataProvider,QWidget *parent = nullptr);
    ~PowerProfileControl();

    void refresh();

private slots:

    void on_radioButton_PPPerformance_clicked();

    void on_radioButton_PPCustom_clicked();

    void on_radioButton_PPBalanced_clicked();

    void on_radioButton_PPQuiet_clicked();

signals:

    void widgetEvent(const LenovoLegionGui::WidgetMessage& event);

private:

    void renderData();

    Ui::PowerProfileControl *ui;

    PowerProfileControlDataProvider * m_dataProvider;

    /*
     *  SysFs values
     */
    LenovoLegionDaemon::PowerProfile::Control::DataInfo    m_powerProfileControlData;
    LenovoLegionDaemon::Batery::DataInfo                   m_batteryControlData;
};

}
