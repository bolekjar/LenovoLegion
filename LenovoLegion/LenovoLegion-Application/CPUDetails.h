// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#pragma once

#include <QWidget>


#include "../LenovoLegion-PrepareBuild/ComputerInfo.pb.h"
#include "../LenovoLegion-PrepareBuild/CPUTopology.pb.h"

namespace Ui {
class CPUDetails;
}


namespace LenovoLegionGui {

class CPUDetails : public QWidget
{
    Q_OBJECT

public:
    explicit CPUDetails(QWidget *parent = nullptr);
    ~CPUDetails();


    void init(const legion::messages::CPUInfo &data,const legion::messages::CPUTopology &topology,int freqMin,int freqMax);

    void closeEvent(QCloseEvent *event) override;

signals:

    void closed();

private:
    Ui::CPUDetails *ui;
};

}
