// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "BateryStatus.h"
#include "ui_BateryStatus.h"

#include "BateryStatusDataProvider.h"

namespace LenovoLegionGui {
BateryStatus::BateryStatus(BateryStatusDataProvider* dataProvider,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BateryStatus)
    , m_dataProvider(dataProvider)
{
    ui->setupUi(this);

    refresh();
}

BateryStatus::~BateryStatus()
{
    delete ui;
}

void BateryStatus::refresh()
{
    m_bateryControlData = m_dataProvider->getBateryControlData();
    renderData();
}

void BateryStatus::renderData()
{
    if(m_bateryControlData.m_isAvailable)
    {
        ui->label_bateryText->setText(QString(m_bateryControlData.m_data.m_batteryStatus.data()).trimmed());
    }
    else
    {
        ui->label_bateryText->setText(QString("N/A"));
    }
}

}
