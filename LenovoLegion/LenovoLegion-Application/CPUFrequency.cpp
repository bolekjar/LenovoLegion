// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "CPUFrequency.h"
#include "ui_CPUFrequency.h"

#include <QCloseEvent>

namespace LenovoLegionGui {

    CPUFrequency::CPUFrequency(QWidget *parent)
        : QWidget(parent)
        , ui(new Ui::CPUFrequency)
        , m_gridLayoutCPUEfficient(nullptr)
    {
        ui->setupUi(this);
    }

    CPUFrequency::~CPUFrequency()
    {
        delete ui;
    }

    void CPUFrequency::addPerformanceWidget(int i, QWidget *widget)
    {
        ui->gridLayout->addWidget(widget,i % 8,i/8);
    }

    void CPUFrequency::addEfficiencyWidget(int i, QWidget *widget)
    {
        ui->gridLayout->addWidget(widget,i % 8,i/8);
    }

    QWidget *CPUFrequency::getPerforamnceWidget(int i)
    {
        return ui->gridLayout->itemAtPosition(i % 8,i/8)->widget();
    }

    QWidget *CPUFrequency::getEfficiencyWidget(int i)
    {
        return ui->gridLayout->itemAtPosition(i % 8,i/8)->widget();
    }

    void CPUFrequency::closeEvent(QCloseEvent *event)
    {
        emit closed();
        hide();
        event->ignore();
    }
}
