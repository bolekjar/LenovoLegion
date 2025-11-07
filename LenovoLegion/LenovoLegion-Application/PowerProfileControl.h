// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include "WidgetMessage.h"


#include "../LenovoLegion-PrepareBuild/PowerProfile.pb.h"
#include "../LenovoLegion-PrepareBuild/Battery.pb.h"

#include <QWidget>

namespace Ui {
class PowerProfileControl;
}

namespace LenovoLegionGui {

class DataProvider;

class PowerProfileControl : public QWidget
{
    Q_OBJECT

public:
    explicit PowerProfileControl(DataProvider * dataProvider,QWidget *parent = nullptr);
    ~PowerProfileControl();

    void refresh();

private slots:

    void on_radioButton_PPPerformance_clicked();

    void on_radioButton_PPCustom_clicked();

    void on_radioButton_PPBalanced_clicked();

    void on_radioButton_PPQuiet_clicked();

    void on_radioButton_PPExtreme_clicked();

signals:

    void widgetEvent(const LenovoLegionGui::WidgetMessage& event);

private:

    void renderData();

    Ui::PowerProfileControl *ui;

    DataProvider * m_dataProvider;

    /*
     *  SysFs values
     */
    legion::messages::PowerProfile                         m_powerProfileControlData;
    legion::messages::Battery                              m_batteryControlData;
};

}
