// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */
#include "FanControl.h"
#include "ui_FanControl.h"

#include <FanControlDataProvider.h>
#include <MainWindow.h>


namespace LenovoLegionGui {

const QString FanControl::NAME = "Fan Control";
const QMap<quint8,QString> FanControl::FAN_CURVE_POINTS_TO_CPU_GPU_FAN_SPEED_TOOLTIP = {
    {0,
QString("CPU fan speed: 1700 rpm\n\
GPU fan speed: 1700 rpm")},
    {1,
QString("CPU fan speed: 1900 rpm\n\
GPU fan speed: 1800 rpm")},
    {2,
QString("CPU fan speed: 2100 rpm\n\
GPU fan speed: 2100 rpm")},
    {3,
QString("CPU fan speed: 2200 rpm\n\
GPU fan speed: 2300 rpm")},
    {4,
QString("CPU fan speed: 2600 rpm\n\
GPU fan speed: 2600 rpm")},
    {5,
QString("CPU fan speed: 2800 rpm\n\
GPU fan speed: 2800 rpm")},
    {6,
QString("CPU fan speed: 3300 rpm\n\
GPU fan speed: 3300 rpm")},
    {7,
QString("CPU fan speed: 3500 rpm\n\
GPU fan speed: 3600 rpm")},
    {8,
QString("CPU fan speed: 4400 rpm\n\
GPU fan speed: 4400 rpm")},
    {9,
QString("CPU fan speed: 5100rpm\n\
GPU fan speed: 5100rpm\n")}
};

FanControl::FanControl(FanControlDataProvider *dataProvider, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FanControl)
    , m_dataProvider(dataProvider)
{
    ui->setupUi(this);

    m_fanControlData           = m_dataProvider->getFanControlData();
    m_localFanCurveControlData = m_fanCurveControlData = m_dataProvider->getFanCurveControlData();

    renderData();
}

FanControl::~FanControl()
{
    delete ui;
}

void FanControl::refresh()
{
    renderData();
}

void FanControl::on_checkBox_MaxFanSpeed_checkStateChanged(const Qt::CheckState &arg1)
{
    m_dataProvider->setFanControlData(LenovoLegionDaemon::FanControl::Control::DataControl{
                                                                                           .m_data = {
                                                                                                .m_maxFanSpeed = (arg1 == Qt::CheckState::Checked ? true : false),
                                                                                                .m_LockFan = m_fanControlData.m_data.m_LockFan
                                                                                            }
    });
    m_fanControlData = m_dataProvider->getFanControlData();
    renderFanControlData();
}

void FanControl::on_checkBox_LockFanControl_checkStateChanged(const Qt::CheckState &arg1)
{
    m_dataProvider->setFanControlData(LenovoLegionDaemon::FanControl::Control::DataControl{
                                                                                           .m_data = {
                                                                                                .m_maxFanSpeed = m_fanControlData.m_data.m_maxFanSpeed,
                                                                                                .m_LockFan = (arg1 == Qt::CheckState::Checked ? true : false)
                                                                                            }
    });
    m_fanControlData = m_dataProvider->getFanControlData();
    renderFanControlData();
}

void FanControl::on_verticalSlider_FanCurve1_valueChanged(int value)
{
    m_localFanCurveControlData.m_data.m_points.at(0) = value;
    normalizeCurrentFanCurveControlData(0,value);
    renderFanCurveControlData();
    markChangesFanCurveControlData();
}

void FanControl::on_verticalSlider_FanCurve2_valueChanged(int value)
{
    if(value < 2) {
        renderFanCurveControlData();
        markChangesFanCurveControlData();
        return;
    }

    m_localFanCurveControlData.m_data.m_points.at(1) = value;
    normalizeCurrentFanCurveControlData(1,value);
    renderFanCurveControlData();
    markChangesFanCurveControlData();
}

void FanControl::on_verticalSlider_FanCurve3_valueChanged(int value)
{
    if(value < 3) {
        renderFanCurveControlData();
        markChangesFanCurveControlData();
        return;
    }

    m_localFanCurveControlData.m_data.m_points.at(2) = value;
    normalizeCurrentFanCurveControlData(2,value);
    renderFanCurveControlData();
    markChangesFanCurveControlData();
}

void FanControl::on_verticalSlider_FanCurve4_valueChanged(int value)
{
    if(value < 4) {
        renderFanCurveControlData();
        markChangesFanCurveControlData();
        return;
    }

    m_localFanCurveControlData.m_data.m_points.at(3) = value;
    normalizeCurrentFanCurveControlData(3,value);
    renderFanCurveControlData();
    markChangesFanCurveControlData();
}

void FanControl::on_verticalSlider_FanCurve5_valueChanged(int value)
{
    if(value < 5) {
        renderFanCurveControlData();
        markChangesFanCurveControlData();
        return;
    }

    m_localFanCurveControlData.m_data.m_points.at(4) = value;
    normalizeCurrentFanCurveControlData(4,value);
    renderFanCurveControlData();
    markChangesFanCurveControlData();
}

void FanControl::on_verticalSlider_FanCurve6_valueChanged(int value)
{
    if(value < 6) {
        renderFanCurveControlData();
        markChangesFanCurveControlData();
        return;
    }

    m_localFanCurveControlData.m_data.m_points.at(5) = value;
    normalizeCurrentFanCurveControlData(5,value);
    renderFanCurveControlData();
    markChangesFanCurveControlData();
}

void FanControl::on_verticalSlider_FanCurve7_valueChanged(int value)
{
    if(value < 7) {
        renderFanCurveControlData();
        markChangesFanCurveControlData();
        return;
    }

    m_localFanCurveControlData.m_data.m_points.at(6) = value;
    normalizeCurrentFanCurveControlData(6,value);
    renderFanCurveControlData();
    markChangesFanCurveControlData();
}

void FanControl::on_verticalSlider_FanCurve8_valueChanged(int value)
{
    if(value < 8) {
        renderFanCurveControlData();
        markChangesFanCurveControlData();
        return;
    }

    m_localFanCurveControlData.m_data.m_points.at(7) = value;
    normalizeCurrentFanCurveControlData(7,value);
    renderFanCurveControlData();
    markChangesFanCurveControlData();
}

void FanControl::on_verticalSlider_FanCurve9_valueChanged(int value)
{
    if(value < 9) {
        renderFanCurveControlData();
        markChangesFanCurveControlData();
        return;
    }
    m_localFanCurveControlData.m_data.m_points.at(8) = value;
    normalizeCurrentFanCurveControlData(8,value);
    renderFanCurveControlData();
    markChangesFanCurveControlData();
}

void FanControl::on_verticalSlider_FanCurve10_valueChanged(int value)
{
    if(value < 10) {
        renderFanCurveControlData();
        markChangesFanCurveControlData();
        return;
    }

    m_localFanCurveControlData.m_data.m_points.at(9) = value;
    normalizeCurrentFanCurveControlData(9,value);
    renderFanCurveControlData();
    markChangesFanCurveControlData();
}

void FanControl::renderData()
{
    renderFanCurveControlData();
    renderFanControlData();
}

void FanControl::renderFanCurveControlData()
{
    ui->verticalSlider_FanCurve1->setValue(m_localFanCurveControlData.m_data.m_points.at(0));
    ui->verticalSlider_FanCurve1->setToolTip(
"CPU temp: 61 C\n\
GPU temp: 58 ℃\n\
CPU sensor temp: 40 ℃\n" + FAN_CURVE_POINTS_TO_CPU_GPU_FAN_SPEED_TOOLTIP.value(m_localFanCurveControlData.m_data.m_points.at(0)-1));
    ui->verticalSlider_FanCurve2->setValue(m_localFanCurveControlData.m_data.m_points.at(1));
    ui->verticalSlider_FanCurve2->setToolTip(
"CPU temp: 61 ℃\n\
GPU temp: 58 ℃\n\
CPU sensor temp: 43 ℃\n" + FAN_CURVE_POINTS_TO_CPU_GPU_FAN_SPEED_TOOLTIP.value(m_localFanCurveControlData.m_data.m_points.at(1)-1));
    ui->verticalSlider_FanCurve3->setValue(m_localFanCurveControlData.m_data.m_points.at(2));
    ui->verticalSlider_FanCurve3->setToolTip(
"CPU temp: 61 ℃\n\
GPU temp: 58 ℃\n\
CPU sensor temp: 46 ℃\n" + FAN_CURVE_POINTS_TO_CPU_GPU_FAN_SPEED_TOOLTIP.value(m_localFanCurveControlData.m_data.m_points.at(2)-1));
    ui->verticalSlider_FanCurve4->setValue(m_localFanCurveControlData.m_data.m_points.at(3));
    ui->verticalSlider_FanCurve4->setToolTip(
"CPU temp: 61 ℃\n\
GPU temp: 58 ℃\n\
CPU sensor temp: N/A\n" + FAN_CURVE_POINTS_TO_CPU_GPU_FAN_SPEED_TOOLTIP.value(m_localFanCurveControlData.m_data.m_points.at(3)-1));
    ui->verticalSlider_FanCurve5->setValue(m_localFanCurveControlData.m_data.m_points.at(4));
    ui->verticalSlider_FanCurve5->setToolTip(
"CPU temp: 61 ℃\n\
GPU temp: 61 ℃\n\
CPU sensor temp: N/A\n" + FAN_CURVE_POINTS_TO_CPU_GPU_FAN_SPEED_TOOLTIP.value(m_localFanCurveControlData.m_data.m_points.at(4)-1));
    ui->verticalSlider_FanCurve6->setValue(m_localFanCurveControlData.m_data.m_points.at(5));
    ui->verticalSlider_FanCurve6->setToolTip(
"CPU temp: 65 ℃\n\
GPU temp: 65 ℃\n\
CPU sensor temp: N/A\n" + FAN_CURVE_POINTS_TO_CPU_GPU_FAN_SPEED_TOOLTIP.value(m_localFanCurveControlData.m_data.m_points.at(5)-1));
    ui->verticalSlider_FanCurve7->setValue(m_localFanCurveControlData.m_data.m_points.at(6));
    ui->verticalSlider_FanCurve7->setToolTip(
"CPU temp: 75 ℃\n\
GPU temp: 72 ℃\n\
CPU sensor temp: N/A\n" + FAN_CURVE_POINTS_TO_CPU_GPU_FAN_SPEED_TOOLTIP.value(m_localFanCurveControlData.m_data.m_points.at(6)-1));
    ui->verticalSlider_FanCurve8->setValue(m_localFanCurveControlData.m_data.m_points.at(7));
    ui->verticalSlider_FanCurve8->setToolTip(
"CPU temp: 77 ℃\n\
GPU temp: 72 ℃\n\
CPU sensor temp: N/A\n" + FAN_CURVE_POINTS_TO_CPU_GPU_FAN_SPEED_TOOLTIP.value(m_localFanCurveControlData.m_data.m_points.at(7)-1));
    ui->verticalSlider_FanCurve9->setValue(m_localFanCurveControlData.m_data.m_points.at(8));
    ui->verticalSlider_FanCurve9->setToolTip(
"CPU temp: 85 ℃\n\
GPU temp: 80 ℃\n\
CPU sensor temp: N/A\n" + FAN_CURVE_POINTS_TO_CPU_GPU_FAN_SPEED_TOOLTIP.value(m_localFanCurveControlData.m_data.m_points.at(8)-1));
    ui->verticalSlider_FanCurve10->setValue(m_localFanCurveControlData.m_data.m_points.at(9));
    ui->verticalSlider_FanCurve10->setToolTip(
"CPU temp: 92 ℃\n\
GPU temp: 83 ℃\n\
CPU sensor temp: N/A\n" + FAN_CURVE_POINTS_TO_CPU_GPU_FAN_SPEED_TOOLTIP.value(m_localFanCurveControlData.m_data.m_points.at(9)-1));
}

void FanControl::renderFanControlData()
{
    ui->checkBox_LockFanControl->setCheckState(m_fanControlData.m_data.m_LockFan ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->checkBox_MaxFanSpeed->setCheckState(m_fanControlData.m_data.m_maxFanSpeed ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
}

void FanControl::markChangesFanCurveControlData()
{
    if(ui->verticalSlider_FanCurve1->value() != m_fanCurveControlData.m_data.m_points.at(0))
    {
        ui->label_FanCurve1->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_FanCurve1->setStyleSheet(QString("QLabel { }"));
    }

    if(ui->verticalSlider_FanCurve2->value() != m_fanCurveControlData.m_data.m_points.at(1))
    {
        ui->label_FanCurve2->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_FanCurve2->setStyleSheet(QString("QLabel { }"));
    }

    if(ui->verticalSlider_FanCurve3->value() != m_fanCurveControlData.m_data.m_points.at(2))
    {
        ui->label_FanCurve3->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_FanCurve3->setStyleSheet(QString("QLabel { }"));
    }
    if(ui->verticalSlider_FanCurve4->value() != m_fanCurveControlData.m_data.m_points.at(3))
    {
        ui->label_FanCurve4->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_FanCurve4->setStyleSheet(QString("QLabel { }"));
    }
    if(ui->verticalSlider_FanCurve5->value() != m_fanCurveControlData.m_data.m_points.at(4))
    {
        ui->label_FanCurve5->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_FanCurve5->setStyleSheet(QString("QLabel { }"));
    }
    if(ui->verticalSlider_FanCurve6->value() != m_fanCurveControlData.m_data.m_points.at(5))
    {
        ui->label_FanCurve6->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_FanCurve6->setStyleSheet(QString("QLabel { }"));
    }
    if(ui->verticalSlider_FanCurve7->value() != m_fanCurveControlData.m_data.m_points.at(6))
    {
        ui->label_FanCurve7->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_FanCurve7->setStyleSheet(QString("QLabel { }"));
    }
    if(ui->verticalSlider_FanCurve8->value() != m_fanCurveControlData.m_data.m_points.at(7))
    {
        ui->label_FanCurve8->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_FanCurve8->setStyleSheet(QString("QLabel { }"));
    }
    if(ui->verticalSlider_FanCurve9->value() != m_fanCurveControlData.m_data.m_points.at(8))
    {
        ui->label_FanCurve9->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_FanCurve9->setStyleSheet(QString("QLabel { }"));
    }
    if(ui->verticalSlider_FanCurve10->value() != m_fanCurveControlData.m_data.m_points.at(9))
    {
        ui->label_FanCurve10->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_FanCurve10->setStyleSheet(QString("QLabel { }"));
    }
}

void FanControl::normalizeCurrentFanCurveControlData(int fromIndex, int value)
{
    for (int var = fromIndex; var < LenovoLegionDaemon::FanControl::CurveControl::MAX_FANCURVE_SIZE; ++var) {
        if(m_localFanCurveControlData.m_data.m_points.at(var) < value)
        {
            m_localFanCurveControlData.m_data.m_points.at(var) = value;
        }
    }

    for (int var = std::min(static_cast<int>(LenovoLegionDaemon::FanControl::CurveControl::MAX_FANCURVE_SIZE),fromIndex); var > -1; --var) {
        if(m_localFanCurveControlData.m_data.m_points.at(var) > value)
        {
            m_localFanCurveControlData.m_data.m_points.at(var) = value;
        }
    }
}

void FanControl::on_pushButton_FanCurveApply_clicked()
{
    m_dataProvider->setFanCurveControlData( {.m_data = { m_localFanCurveControlData.m_data}});
    m_localFanCurveControlData = m_fanCurveControlData = m_dataProvider->getFanCurveControlData();
    renderFanCurveControlData();
    markChangesFanCurveControlData();
}

void FanControl::on_pushButton_FanCurveCancel_clicked()
{
    m_localFanCurveControlData = m_fanCurveControlData;
    renderFanCurveControlData();
}















}
