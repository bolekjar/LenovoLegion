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
class FanControl;
}

namespace LenovoLegionGui {

class FanControlDataProvider;


class FanControl : public QWidget
{
    Q_OBJECT

public:

    static const QString              NAME;
    static const QMap<quint8,QString> FAN_CURVE_POINTS_TO_CPU_GPU_FAN_SPEED_TOOLTIP;

public:
    explicit FanControl(FanControlDataProvider *dataProvider,QWidget *parent = nullptr);
    ~FanControl();

    void refresh();

signals:
    void widgetEvent(const LenovoLegionGui::WidgetMessage& event);
private slots:


    void on_checkBox_MaxFanSpeed_checkStateChanged(const Qt::CheckState &arg1);
    void on_checkBox_LockFanControl_checkStateChanged(const Qt::CheckState &arg1);

    void on_verticalSlider_FanCurve1_valueChanged(int value);
    void on_verticalSlider_FanCurve2_valueChanged(int value);
    void on_verticalSlider_FanCurve3_valueChanged(int value);
    void on_verticalSlider_FanCurve4_valueChanged(int value);
    void on_verticalSlider_FanCurve5_valueChanged(int value);
    void on_verticalSlider_FanCurve6_valueChanged(int value);
    void on_verticalSlider_FanCurve7_valueChanged(int value);
    void on_verticalSlider_FanCurve8_valueChanged(int value);
    void on_verticalSlider_FanCurve9_valueChanged(int value);
    void on_verticalSlider_FanCurve10_valueChanged(int value);

    void on_pushButton_FanCurveApply_clicked();
    void on_pushButton_FanCurveCancel_clicked();

private:

    void renderData();
    void renderFanCurveControlData();
    void renderFanControlData();

    void markChangesFanCurveControlData();

    void normalizeCurrentFanCurveControlData(int fromIndex,int value);

    void setEnabledGuiElementsFanCurveControl(bool enabled);
    void setEnabledGuiElementsMaxFanSpeedControl(bool enabled);
    void setEnabledGuiElementsLockFanControl(bool enabled);

private:
    Ui::FanControl *ui;

    FanControlDataProvider                        *m_dataProvider;

    LenovoLegionDaemon::FanControl::Control::DataInfo             m_fanControlData;
    LenovoLegionDaemon::FanControl::CurveControl::DataInfo        m_fanCurveControlData,
                                                                  m_localFanCurveControlData;
};

}
