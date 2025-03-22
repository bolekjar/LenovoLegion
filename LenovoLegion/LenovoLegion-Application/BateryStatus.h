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
class BateryStatus;
}

namespace LenovoLegionGui {

class BateryStatusDataProvider;

class BateryStatus : public QWidget
{
    Q_OBJECT

public:
    explicit BateryStatus(BateryStatusDataProvider* dataProvider,QWidget *parent = nullptr);

    ~BateryStatus();

    void refresh();


signals:

    void widgetEvent(const LenovoLegionGui::WidgetMessage& event);

private:

    void renderData();

private:

    Ui::BateryStatus *ui;

    BateryStatusDataProvider* m_dataProvider;

    LenovoLegionDaemon::Batery::DataInfo m_bateryControlData;
};

}
