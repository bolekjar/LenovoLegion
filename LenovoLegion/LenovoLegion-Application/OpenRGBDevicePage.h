// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#pragma once

#include "../LenovoLegion-Daemon/RGBControllerInterface.h"

#include <QFrame>

#include <memory>

namespace Ui
{
    class OpenRGBDevicePage;
}

    class QListWidgetItem;
namespace LenovoLegionGui {

class OpenRGBDevicePage : public QFrame
{
    Q_OBJECT

public:
    explicit OpenRGBDevicePage(LenovoLegionDaemon::RGBControllerInterface *dev, QWidget *parent = nullptr);
    ~OpenRGBDevicePage();
private:
    void SetDevice(unsigned char red, unsigned char green, unsigned char blue);

    void UpdateModeUi(unsigned int selectColorMode = std::numeric_limits<unsigned int>::max());
    void UpdateEffectUi(unsigned int selectEffectIndx = 0, unsigned int selectModeColorIdx = 0);

    void ShowDeviceView();
    void HideDeviceView();
private slots:

    bool eventFilter(QObject* watched, QEvent* event);

    /*
     * Update the entire user interface
     */
    void UpdateInterface();

    /*
     * Color selection changed event handlers
     */
    void on_ColorWheelBox_colorChanged(const QColor color);
    void on_SwatchBox_swatchChanged(const QColor color);
    void on_BrightnessSlider_valueChanged(int value);
    void on_RedSpinBox_valueChanged(int red);
    void on_HueSpinBox_valueChanged(int hue);
    void on_GreenSpinBox_valueChanged(int green);
    void on_SatSpinBox_valueChanged(int sat);
    void on_BlueSpinBox_valueChanged(int blue);
    void on_ValSpinBox_valueChanged(int val);
    void on_HexLineEdit_textChanged(const QString &arg1);


    /*
     * Device view
     */
    void on_DeviceViewBox_selectionChanged(QVector<int>);
    void on_pushButtonToggleLEDView_clicked();

    /*
     * Profile changed event handler
     */
    void on_ProfileBox_currentIndexChanged(int index);

    /*
     * Add/Remove effects event handlers
     */
    void on_listWidgetEffects_currentRowChanged(int currentRow);
    void on_listWidgetEffects_itemEntered(QListWidgetItem *item);
    void on_comboBox_EffectsColors_currentIndexChanged(int index);
    void on_pushButton_EffectsClearAll_clicked();
    void on_pushButton_EffectDelete_clicked();
    void on_pushButton_AddEffects_clicked();
    void on_pushButton_EffectsDefault_clicked();

    /*
     * Mode change event handlers
     */
    void on_ModeBox_currentIndexChanged(int index);
    void on_ModeSpecificCheck_clicked();


    void on_comboBox_modeSpecificColor_currentIndexChanged(int index);
    void on_PerLEDCheck_clicked();
    void on_RandomCheck_clicked();


private:
    Ui::OpenRGBDevicePage *ui;
    std::unique_ptr<LenovoLegionDaemon::RGBControllerInterface> device;

    bool UpdateHex          = true;
    bool HexFormatRGB       = true;

    QColor current_color;
    void updateColorUi();
    void colorChanged();


    QString ModeDescription(const LenovoLegionDaemon::mode& m);
};

}
