#include "OffsetsControl.h"
#include "ui_OffsetsControl.h"

#include "MainWindow.h"

#include "DataProvider.h"

#include "../LenovoLegion-Daemon/SysFsDataProviderIntelMSR.h"
#include "../LenovoLegion-Daemon/DataProviderNvidiaNvml.h"

namespace LenovoLegionGui {

const QString OffsetsControl::NAME = "Offset Control";;

const QMap<QString, legion::messages::CpuIntelMSR> OffsetsControl::CPU_VOLTAGE_PRESETS = {
    {"None",{}},
    {"Conservative",[](){
            legion::messages::CpuIntelMSR voltage;

            voltage.mutable_cpu()->set_offset(-50);
            voltage.mutable_uncore()->set_offset(-50);
            voltage.mutable_cache()->set_offset(-50);
            voltage.mutable_gpu()->set_offset(-40);
            voltage.mutable_analogio()->set_offset(0);

            return voltage;
        }(),},
    {"Moderate",[](){
         legion::messages::CpuIntelMSR voltage;

         voltage.mutable_cpu()->set_offset(-100);
         voltage.mutable_uncore()->set_offset(-80);
         voltage.mutable_cache()->set_offset(-100);
         voltage.mutable_gpu()->set_offset(-75);
         voltage.mutable_analogio()->set_offset(0);

         return voltage;
     }()},
    {"Aggressive",[](){
         legion::messages::CpuIntelMSR voltage;

         voltage.mutable_cpu()->set_offset(-150);
         voltage.mutable_uncore()->set_offset(-100);
         voltage.mutable_cache()->set_offset(-150);
         voltage.mutable_gpu()->set_offset(-100);
         voltage.mutable_analogio()->set_offset(0);

         return voltage;
     }()},
    {"Reset",[](){
         legion::messages::CpuIntelMSR voltage;

         voltage.mutable_cpu()->set_offset(0);
         voltage.mutable_uncore()->set_offset(0);
         voltage.mutable_cache()->set_offset(0);
         voltage.mutable_gpu()->set_offset(0);
         voltage.mutable_analogio()->set_offset(0);

         return voltage;
     }()},
};

const QMap<QString, legion::messages::NvidiaNvml> OffsetsControl::GPU_CLOCK_PRESETS{
    {"None",{}},
    {"Mild Overclock",[](){
            legion::messages::NvidiaNvml clock;

            clock.mutable_gpu_offset()->set_value(100);
            clock.mutable_memory_offset()->set_value(200);

            return clock;
        }(),},
    {"Moderate Overclock",[](){
         legion::messages::NvidiaNvml clock;

         clock.mutable_gpu_offset()->set_value(150);
         clock.mutable_memory_offset()->set_value(300);

         return clock;
     }()},
    {"Extreme Overclock",[](){
         legion::messages::NvidiaNvml clock;

         clock.mutable_gpu_offset()->set_value(250);
         clock.mutable_memory_offset()->set_value(500);

         return clock;
     }()},
    {"Reset",[](){
         legion::messages::NvidiaNvml clock;

         clock.mutable_gpu_offset()->set_value(0);
         clock.mutable_memory_offset()->set_value(0);
         return clock;
     }()}
};

OffsetsControl::OffsetsControl(DataProvider *dataProvider,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OffsetsControl)
    , m_dataProvider(dataProvider)
{
    ui->setupUi(this);

    readCpuIntelMsrData();
    readGpuNvmlData();

    if(!m_cpuIntelMSRData.has_analogio() ||
       !m_cpuIntelMSRData.has_cache()    ||
       !m_cpuIntelMSRData.has_cpu()      ||
       !m_cpuIntelMSRData.has_gpu()      ||
       !m_cpuIntelMSRData.has_uncore()
       )
    {
        THROW_EXCEPTION(exception_T, ERROR_CODES::DATA_NOT_READY, "Voltage Control data not available");
    }

    if(!m_gpuNvmlData.has_gpu_offset() ||
       !m_gpuNvmlData.has_memory_offset()
       )
    {
        THROW_EXCEPTION(exception_T, ERROR_CODES::DATA_NOT_READY, "GPU Offset data not available");
    }


    ui->horizontalSlider_cpuVoltageCore->setMinimum(-m_cpuIntelMSRData.cpu().max_undervolt());
    ui->horizontalSlider_cpuVoltageCore->setMaximum(m_cpuIntelMSRData.cpu().max_overvolt());

    ui->horizontalSlider_cpuVoltageCache->setMinimum(-m_cpuIntelMSRData.cache().max_undervolt());
    ui->horizontalSlider_cpuVoltageCache->setMaximum(m_cpuIntelMSRData.cache().max_overvolt());

    ui->horizontalSlider_cpuVoltageGPU->setMinimum(-m_cpuIntelMSRData.gpu().max_undervolt());
    ui->horizontalSlider_cpuVoltageGPU->setMaximum(m_cpuIntelMSRData.gpu().max_overvolt());

    ui->horizontalSlider_cpuVoltageUncore->setMinimum(-m_cpuIntelMSRData.uncore().max_undervolt());
    ui->horizontalSlider_cpuVoltageUncore->setMaximum(m_cpuIntelMSRData.uncore().max_overvolt());

    ui->horizontalSlider_cpuVoltageAnalogIO->setMinimum(-m_cpuIntelMSRData.analogio().max_undervolt());
    ui->horizontalSlider_cpuVoltageAnalogIO->setMaximum(m_cpuIntelMSRData.analogio().max_overvolt());

    ui->horizontalSlider_GPUClockOffset->setMinimum(m_gpuNvmlData.gpu_offset().min());
    ui->horizontalSlider_GPUClockOffset->setMaximum(m_gpuNvmlData.gpu_offset().max());

    ui->horizontalSlider_GPUMemoryOffset->setMinimum(m_gpuNvmlData.memory_offset().min());
    ui->horizontalSlider_GPUMemoryOffset->setMaximum(m_gpuNvmlData.memory_offset().max());


    for (auto presetNameI = CPU_VOLTAGE_PRESETS.constBegin(); presetNameI != CPU_VOLTAGE_PRESETS.constEnd(); ++presetNameI)
    {
        ui->comboBox_cpuVoltagePreset->addItem(presetNameI.key());
    };


    for (auto presetNameI = GPU_CLOCK_PRESETS.constBegin(); presetNameI != GPU_CLOCK_PRESETS.constEnd(); ++presetNameI)
    {
        ui->comboBox_GPUPreset->addItem(presetNameI.key());
    };


    refresh();
}

OffsetsControl::~OffsetsControl()
{
    delete ui;
}

void OffsetsControl::refresh()
{
    refreshCpuVoltageOffsetData();
    refreshGpuOffsetData();

    markChanges();
}

void OffsetsControl::on_horizontalSlider_cpuVoltageCore_valueChanged(int value)
{
    if((value % 10) == 0)
    {
        ui->lcdNumber_cpuVoltageCore->display(value);
        markChangesCpuVoltageOffsetData();
        return;
    }

    ui->horizontalSlider_cpuVoltageCore->setValue(value - (value % 10));

    markChangesCpuVoltageOffsetData();
}

void OffsetsControl::on_horizontalSlider_cpuVoltageUncore_valueChanged(int value)
{
    if((value % 10) == 0)
    {
        ui->lcdNumber_cpuVoltageUncore->display(value);
        markChangesCpuVoltageOffsetData();
        return;
    }

    ui->horizontalSlider_cpuVoltageUncore->setValue(value - (value % 10));
}

void OffsetsControl::on_horizontalSlider_cpuVoltageCache_valueChanged(int value)
{
    if((value % 10) == 0)
    {
        ui->lcdNumber_cpuVoltageCache->display(value);
        markChangesCpuVoltageOffsetData();
        return;
    }

    ui->horizontalSlider_cpuVoltageCache->setValue(value - (value % 10));

    markChangesCpuVoltageOffsetData();
}

void OffsetsControl::on_horizontalSlider_cpuVoltageGPU_valueChanged(int value)
{
    if((value % 10) == 0)
    {
        ui->lcdNumber_cpuVoltageGPU->display(value);
        markChangesCpuVoltageOffsetData();
        return;
    }

    ui->horizontalSlider_cpuVoltageGPU->setValue(value - (value % 10));

    markChangesCpuVoltageOffsetData();
}

void OffsetsControl::on_horizontalSlider_cpuVoltageAnalogIO_valueChanged(int value)
{
    if((value % 10) == 0)
    {
        ui->lcdNumber_cpuVoltageAnalogIO->display(value);
        markChangesCpuVoltageOffsetData();
        return;
    }

    ui->horizontalSlider_cpuVoltageAnalogIO->setValue(value - (value % 10));

    markChangesCpuVoltageOffsetData();
}

void OffsetsControl::on_pushButton_cpuVoltageCancel_clicked()
{
    refreshCpuVoltageOffsetData();
}

void OffsetsControl::on_pushButton_cpuVoltageApply_clicked()
{
    legion::messages::CpuIntelMSR cpuIntelMSR;

    cpuIntelMSR.mutable_cpu()->set_offset(ui->horizontalSlider_cpuVoltageCore->value() * 1000);
    cpuIntelMSR.mutable_uncore()->set_offset(ui->horizontalSlider_cpuVoltageUncore->value() * 1000);
    cpuIntelMSR.mutable_cache()->set_offset(ui->horizontalSlider_cpuVoltageCache->value() * 1000);
    cpuIntelMSR.mutable_gpu()->set_offset(ui->horizontalSlider_cpuVoltageGPU->value()* 1000);
    cpuIntelMSR.mutable_analogio()->set_offset(ui->horizontalSlider_cpuVoltageAnalogIO->value() * 1000);

    m_dataProvider->setDataMessage(LenovoLegionDaemon::SysFsDataProviderIntelMSR::dataType,cpuIntelMSR);

    readCpuIntelMsrData();

    refreshCpuVoltageOffsetData();
}

void OffsetsControl::markChangesCpuVoltageOffsetData()
{
    if(static_cast<int>(ui->horizontalSlider_cpuVoltageCore->value()) != m_cpuIntelMSRData.cpu().offset())
    {
        ui->label_cpuVoltageCore->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_cpuVoltageCore->setStyleSheet(QString("QLabel { }"));
    }

    if(static_cast<int>(ui->horizontalSlider_cpuVoltageUncore->value()) != m_cpuIntelMSRData.uncore().offset())
    {
        ui->label_cpuVoltageUncore->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_cpuVoltageUncore->setStyleSheet(QString("QLabel { }"));
    }

    if(static_cast<int>(ui->horizontalSlider_cpuVoltageCache->value()) != m_cpuIntelMSRData.cache().offset())
    {
        ui->label_cpuVoltageCache->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_cpuVoltageCache->setStyleSheet(QString("QLabel { }"));
    }

    if(static_cast<int>(ui->horizontalSlider_cpuVoltageGPU->value()) != m_cpuIntelMSRData.gpu().offset())
    {
        ui->label_cpuVoltageGPU->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_cpuVoltageGPU->setStyleSheet(QString("QLabel { }"));
    }

    if(static_cast<int>(ui->horizontalSlider_cpuVoltageAnalogIO->value()) != m_cpuIntelMSRData.analogio().offset())
    {
        ui->label_cpuVoltageAnalogIO->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_cpuVoltageAnalogIO->setStyleSheet(QString("QLabel { }"));
    }
}

void OffsetsControl::markChangesGpuOffsetData()
{
    if(ui->horizontalSlider_GPUClockOffset->value() != m_gpuNvmlData.gpu_offset().value())
    {
        ui->label_GPUClockOffset->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_GPUClockOffset->setStyleSheet(QString("QLabel { }"));
    }

    if(ui->horizontalSlider_GPUMemoryOffset->value() != m_gpuNvmlData.memory_offset().value())
    {
        ui->label_GPUMemoryOffset->setStyleSheet(QString("QLabel {  color : ").append(MainWindow::VALUE_CHANGE_COLOR).append("; }"));
    }
    else {
        ui->label_GPUMemoryOffset->setStyleSheet(QString("QLabel { }"));
    }
}

void OffsetsControl::markChanges()
{
    markChangesCpuVoltageOffsetData();
    markChangesGpuOffsetData();
}

void OffsetsControl::readCpuIntelMsrData()
{
    m_cpuIntelMSRData = m_dataProvider->getDataMessage<legion::messages::CpuIntelMSR> (LenovoLegionDaemon::SysFsDataProviderIntelMSR::dataType);

    /*
     * Normalize => mV
     */
    m_cpuIntelMSRData.mutable_cpu()->set_offset((m_cpuIntelMSRData.cpu().offset() > 0 ? m_cpuIntelMSRData.cpu().offset() + 999 :m_cpuIntelMSRData.cpu().offset() - 999)/ 1000);
    m_cpuIntelMSRData.mutable_cpu()->set_max_undervolt((m_cpuIntelMSRData.cpu().max_undervolt() + 999 )/ 1000);
    m_cpuIntelMSRData.mutable_cpu()->set_max_overvolt((m_cpuIntelMSRData.cpu().max_overvolt() + 999 )/ 1000);

    m_cpuIntelMSRData.mutable_uncore()->set_offset((m_cpuIntelMSRData.uncore().offset() > 0 ? m_cpuIntelMSRData.uncore().offset() + 999 : m_cpuIntelMSRData.uncore().offset() - 999) / 1000);
    m_cpuIntelMSRData.mutable_uncore()->set_max_undervolt((m_cpuIntelMSRData.uncore().max_undervolt() + 999) / 1000);
    m_cpuIntelMSRData.mutable_uncore()->set_max_overvolt((m_cpuIntelMSRData.uncore().max_overvolt() + 999) / 1000);

    m_cpuIntelMSRData.mutable_cache()->set_offset((m_cpuIntelMSRData.cache().offset() > 0 ? m_cpuIntelMSRData.cache().offset() + 999 : m_cpuIntelMSRData.cache().offset() - 999) / 1000);
    m_cpuIntelMSRData.mutable_cache()->set_max_undervolt((m_cpuIntelMSRData.cache().max_undervolt() + 999) / 1000);
    m_cpuIntelMSRData.mutable_cache()->set_max_overvolt((m_cpuIntelMSRData.cache().max_overvolt() + 999) / 1000);

    m_cpuIntelMSRData.mutable_gpu()->set_offset((m_cpuIntelMSRData.gpu().offset() > 0 ? m_cpuIntelMSRData.gpu().offset()  + 999 : m_cpuIntelMSRData.gpu().offset() - 999) / 1000);
    m_cpuIntelMSRData.mutable_gpu()->set_max_undervolt((m_cpuIntelMSRData.gpu().max_undervolt() + 999) / 1000);
    m_cpuIntelMSRData.mutable_gpu()->set_max_overvolt((m_cpuIntelMSRData.gpu().max_overvolt() + 999) / 1000);

    m_cpuIntelMSRData.mutable_analogio()->set_offset((m_cpuIntelMSRData.analogio().offset() > 0 ?m_cpuIntelMSRData.analogio().offset() + 999 : m_cpuIntelMSRData.analogio().offset() - 999)/ 1000);
    m_cpuIntelMSRData.mutable_analogio()->set_max_undervolt((m_cpuIntelMSRData.analogio().max_undervolt() + 999) / 1000);
    m_cpuIntelMSRData.mutable_analogio()->set_max_overvolt((m_cpuIntelMSRData.analogio().max_overvolt() + 999) / 1000);
}

void OffsetsControl::readGpuNvmlData()
{
    m_gpuNvmlData = m_dataProvider->getDataMessage<legion::messages::NvidiaNvml>(LenovoLegionDaemon::DataProviderNvidiaNvml::dataType);
}

void OffsetsControl::on_comboBox_cpuVoltagePreset_currentTextChanged(const QString &arg1)
{
    const legion::messages::CpuIntelMSR &preset = CPU_VOLTAGE_PRESETS.value(arg1);

    if(preset.has_cpu())
        ui->horizontalSlider_cpuVoltageCore->setValue(preset.cpu().offset());
    if(preset.has_uncore())
        ui->horizontalSlider_cpuVoltageUncore->setValue(preset.uncore().offset());
    if(preset.has_cache())
        ui->horizontalSlider_cpuVoltageCache->setValue(preset.cache().offset());
    if(preset.has_gpu())
        ui->horizontalSlider_cpuVoltageGPU->setValue(preset.gpu().offset());
    if(preset.has_analogio())
        ui->horizontalSlider_cpuVoltageAnalogIO->setValue(preset.analogio().offset());

    markChangesCpuVoltageOffsetData();
}

void OffsetsControl::refreshCpuVoltageOffsetData()
{
    ui->comboBox_cpuVoltagePreset->setCurrentText("None");

    ui->horizontalSlider_cpuVoltageCore->setValue(m_cpuIntelMSRData.cpu().offset());
    ui->lcdNumber_cpuVoltageCore->display(m_cpuIntelMSRData.cpu().offset());

    ui->horizontalSlider_cpuVoltageCache->setValue(m_cpuIntelMSRData.cache().offset());
    ui->lcdNumber_cpuVoltageCache->display(m_cpuIntelMSRData.cache().offset());

    ui->horizontalSlider_cpuVoltageGPU->setValue(m_cpuIntelMSRData.gpu().offset());
    ui->lcdNumber_cpuVoltageGPU->display(m_cpuIntelMSRData.gpu().offset());

    ui->horizontalSlider_cpuVoltageUncore->setValue(m_cpuIntelMSRData.uncore().offset());
    ui->lcdNumber_cpuVoltageUncore->display(m_cpuIntelMSRData.uncore().offset());

    ui->horizontalSlider_cpuVoltageAnalogIO->setValue(m_cpuIntelMSRData.analogio().offset());
    ui->lcdNumber_cpuVoltageAnalogIO->display(m_cpuIntelMSRData.analogio().offset());


    markChangesCpuVoltageOffsetData();
}

void OffsetsControl::refreshGpuOffsetData()
{
    ui->comboBox_GPUPreset->setCurrentText("None");

    ui->horizontalSlider_GPUClockOffset->setValue(m_gpuNvmlData.gpu_offset().value());
    ui->horizontalSlider_GPUMemoryOffset->setValue(m_gpuNvmlData.memory_offset().value());

    markChangesGpuOffsetData();
}

void OffsetsControl::on_pushButton_GPUCancel_clicked()
{
    refreshGpuOffsetData();
}

void OffsetsControl::on_pushButton_GPUApply_clicked()
{
    legion::messages::NvidiaNvml gpuNvml;

    gpuNvml.mutable_gpu_offset()->set_value(ui->horizontalSlider_GPUClockOffset->value());
    gpuNvml.mutable_memory_offset()->set_value(ui->horizontalSlider_GPUMemoryOffset->value());

    m_dataProvider->setDataMessage(LenovoLegionDaemon::DataProviderNvidiaNvml::dataType,gpuNvml);

    readGpuNvmlData();

    refreshGpuOffsetData();
}

void OffsetsControl::on_comboBox_GPUPreset_currentTextChanged(const QString &arg1)
{
    const legion::messages::NvidiaNvml &preset = GPU_CLOCK_PRESETS.value(arg1);

    if(preset.has_gpu_offset())
        ui->horizontalSlider_GPUClockOffset->setValue(preset.gpu_offset().value());
    if(preset.has_memory_offset())
        ui->horizontalSlider_GPUMemoryOffset->setValue(preset.memory_offset().value());

    markChangesGpuOffsetData();
}

void OffsetsControl::on_horizontalSlider_GPUMemoryOffset_valueChanged(int value)
{
    if((value % 10) == 0)
    {
        ui->lcdNumber_GPUMemoryOffset->display(value);
        markChangesGpuOffsetData();
        return;
    }

    ui->horizontalSlider_GPUMemoryOffset->setValue(value - (value % 10));

    markChangesGpuOffsetData();
}

void OffsetsControl::on_horizontalSlider_GPUClockOffset_valueChanged(int value)
{
    if((value % 10) == 0)
    {
        ui->lcdNumber_GPUClockOffset->display(value);
        markChangesGpuOffsetData();
        return;
    }

    ui->horizontalSlider_GPUClockOffset->setValue(value - (value % 10));

    markChangesGpuOffsetData();
}







}
