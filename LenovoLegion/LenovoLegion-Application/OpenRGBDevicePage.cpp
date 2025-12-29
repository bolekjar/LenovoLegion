// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright Jaroslav Bolek 2025
 *
 * Author(s):
 *   Jaroslav Bolek <jaroslav.bolek@gmail.com>
 */

#include "OpenRGBDevicePage.h"
#include "RGBControllerKeyNames.h"
#include "ui_OpenRGBDevicePage.h"


#include <Core/LoggerHolder.h>


#include <QAbstractItemView>
#include <QScreen>

namespace LenovoLegionGui {


QString OpenRGBDevicePage::ModeDescription(const LenovoLegionDaemon::mode& m)
{
    /*-----------------------------------------------------------------*\
    | List of common mode names can be found on the OpenRGB Wiki:       |
    | https://gitlab.com/CalcProgrammer1/OpenRGB/-/wikis/Common-Modes   |
    \*-----------------------------------------------------------------*/
    static const std::unordered_map<std::string, QString> descriptions =
    {
        {"Direct",          tr("Set individual LEDs to static colors.  Safe for use with software-driven effects.")                     },
        {"Custom",          tr("Set individual LEDs to static colors.  Not safe for use with software-driven effects.")                 },
        {"Static",          tr("Sets the entire device or a zone to a single color.")                                                   },
        {"Breathing",       tr("Gradually fades between fully off and fully on.")                                                       },
        {"Flashing",        tr("Abruptly changes between fully off and fully on.")                                                      },
        {"Spectrum Cycle",  tr("Gradually cycles through the entire color spectrum.  All lights on the device are the same color.")     },
        {"Rainbow Wave",    tr("Gradually cycles through the entire color spectrum.  Produces a rainbow pattern that moves.")           },
        {"Reactive",        tr("Flashes lights when keys or buttons are pressed.")                                                      },
    };

    /*-----------------------------------------------------------------*\
    | Find the given mode name in the list and return the description   |
    | if it exists, otherwise return an empty string                    |
    \*-----------------------------------------------------------------*/
    std::unordered_map<std::string, QString>::const_iterator it = descriptions.find(m.name);

    if(it != descriptions.end())
    {
        return it->second;
    }

    return "";
}

OpenRGBDevicePage::OpenRGBDevicePage(LenovoLegionDaemon::RGBControllerInterface *dev, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::OpenRGBDevicePage)
{
    ui->setupUi(this);

    ui->LEDBox->view()->window()->setWindowFlags( Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::NoDropShadowWindowHint);
    ui->LEDBox->view()->window()->setAttribute(Qt::WA_TranslucentBackground);

    ui->ModeBox->view()->window()->setWindowFlags( Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::NoDropShadowWindowHint);
    ui->ModeBox->view()->window()->setAttribute(Qt::WA_TranslucentBackground);

    ui->DirectionBox->view()->window()->setWindowFlags( Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::NoDropShadowWindowHint);
    ui->DirectionBox->view()->window()->setAttribute(Qt::WA_TranslucentBackground);

    ui->ZoneBox->view()->window()->setWindowFlags( Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::NoDropShadowWindowHint);
    ui->ZoneBox->view()->window()->setAttribute(Qt::WA_TranslucentBackground);

    ui->ProfileBox->view()->window()->setWindowFlags( Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::NoDropShadowWindowHint);
    ui->ProfileBox->view()->window()->setAttribute(Qt::WA_TranslucentBackground);

    ui->DirectionEffectsBox->view()->window()->setWindowFlags( Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::NoDropShadowWindowHint);
    ui->DirectionEffectsBox->view()->window()->setAttribute(Qt::WA_TranslucentBackground);

    ui->LEDEffectsBox->view()->window()->setWindowFlags( Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::NoDropShadowWindowHint);
    ui->LEDEffectsBox->view()->window()->setAttribute(Qt::WA_TranslucentBackground);

    ui->ModeEffectsBox->view()->window()->setWindowFlags( Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::NoDropShadowWindowHint);
    ui->ModeEffectsBox->view()->window()->setAttribute(Qt::WA_TranslucentBackground);

    ui->SpeedEffectsComboBox->view()->window()->setWindowFlags( Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::NoDropShadowWindowHint);
    ui->SpeedEffectsComboBox->view()->window()->setAttribute(Qt::WA_TranslucentBackground);



    /*-----------------------------------------------------*\
    | Store device pointer                                  |
    \*-----------------------------------------------------*/
    device.reset(dev);

    /*-----------------------------------------------------*\
    | Set up the device view                                |
    \*-----------------------------------------------------*/
    ui->DeviceViewBox->setController(device.get());
    ui->DeviceViewBoxFrame->hide();

    connect(ui->DeviceViewBox, &DeviceView::selectionChanged, this, &OpenRGBDevicePage::on_DeviceViewBox_selectionChanged);

    /*-----------------------------------------------------*\
     | The profile selection  box                           |
    \*-----------------------------------------------------*/
    ui->ProfileBox->blockSignals(true);
    ui->ProfileBox->setVisible(false);
    ui->ProfileLabel->setVisible(false);
    ui->ProfileBox->clear();

    auto profiles = device->GetProfiles();
    if(profiles.max > 0)
    {
        /*-----------------------------------------------------*\
        | Fill in the profile selection box                    |
        \*-----------------------------------------------------*/
        for (std::size_t i = profiles.min; i < profiles.max + 1; i++)
        {
            ui->ProfileBox->addItem(QString("Profile ").append(QString::number(i)),QVariant::fromValue(i));          
        }

        ui->ProfileBox->setCurrentIndex(profiles.active - profiles.min);

        ui->ProfileBox->setVisible(true);
        ui->ProfileLabel->setVisible(true);
        ui->ProfileBox->blockSignals(false);
    }




    /*-----------------------------------------------------*\
     | Fill the brightness slider                           |
    \*-----------------------------------------------------*/
    ui->BrightnessSlider->blockSignals(true);
    ui->BrightnessSlider->setVisible(false);
    ui->BrightnessSlider->setEnabled(false);

    auto brightness = device->GetBrightness();
    if(brightness.min < brightness.max)
    {
        ui->BrightnessSlider->setMinimum(brightness.min);
        ui->BrightnessSlider->setMaximum(brightness.max);

        ui->BrightnessSlider->setValue(brightness.active);
        ui->BrightnessSlider->setEnabled(true);
        ui->BrightnessSlider->setVisible(true);
        ui->BrightnessSlider->blockSignals(false);
    }


    /*-----------------------------------------------------*\
    | Fill in the mode selection box                        |
    \*-----------------------------------------------------*/
    ui->ModeBox->blockSignals(true);
    ui->ModeBox->setVisible(false);
    ui->ModeBox->setEnabled(false);
    ui->ModeBox->clear();

    auto& modes = device->GetModes();
    for(std::size_t i = 0; i < modes.size(); i++)
    {
        ui->ModeBox->addItem(device->GetModeNameByIdx(i).c_str());
        ui->ModeBox->setItemData((int)i, ModeDescription(modes[i]), Qt::ToolTipRole);
    }

    if(ui->ModeBox->count() > 0 )
    {
        ui->ModeBox->setCurrentIndex(0);
        ui->ModeBox->blockSignals(false);
        ui->ModeBox->setVisible(true);
        ui->ModeBox->setEnabled(true);
    }

    /*-----------------------------------------------------*\
    | Update mode user interface elements                   |
    \*-----------------------------------------------------*/
    UpdateModeUi();


    /*-----------------------------------------------------*\
     | Update effect user interface elements                |
    \*-----------------------------------------------------*/
    UpdateEffectUi();


    /*-----------------------------------------------------*\
    | Set initial color to black and update UI              |
    \*-----------------------------------------------------*/
    current_color.setRgb(0, 0, 0);
    updateColorUi();
}

OpenRGBDevicePage::~OpenRGBDevicePage()
{
    delete ui;
}

void OpenRGBDevicePage::timerEvent(QTimerEvent *)
{
    // Adjust contrast
    /*float contrast = 1.5;  // 1.0 = no change, >1.0 = more contrast, <1.0 = less contrast

    auto& modes = device->GetModes();
    auto& leds  = device->GetLEDs();
    if(modes[device->GetMode()].flags & LenovoLegionDaemon::MODE_FLAG_HAS_DIRECT_CONTROL)
    {
        QScreen *screen = QGuiApplication::primaryScreen();

        if (screen) {

            QPixmap screenshot = screen->grabWindow(0).scaled(20,8, Qt::IgnoreAspectRatio); //TODO

            for(size_t i = 0; i < leds.size(); i++)
            {
                QColor color = screenshot.toImage().pixelColor(i % 20, i / 20);

                device->SetLED(i, ToRGBColor(
                                       qBound(0, (int)((color.red()   - 128) * contrast + 128), 255),
                                       qBound(0, (int)((color.green() - 128) * contrast + 128), 255),
                                       qBound(0, (int)((color.blue()  - 128) * contrast + 128), 255)
                                       ));
            }

            device->ApplyPendingChanges();
        }

    }*/
}

void OpenRGBDevicePage::on_BrightnessSlider_valueChanged(int value)
{
    /*-----------------------------------------------------*\
    | Set device brightness                                 |
    \*-----------------------------------------------------*/
    device->SetBrightness((unsigned int)value);

    device->ApplyPendingChanges();
}

void OpenRGBDevicePage::UpdateInterface()
{
    /*-----------------------------------------------------*\
    | Update mode user interface elements                   |
    \*-----------------------------------------------------*/
    UpdateModeUi();


    /*-----------------------------------------------------*\
     | Update effect user interface elements                |
    \*-----------------------------------------------------*/
    UpdateEffectUi();


    repaint();
}

void OpenRGBDevicePage::UpdateModeUi(unsigned int selectColorMode)
{

    /*-----------------------------------------------------*\
     *
     * Block signals to avoid recursive updates
     * -----------------------------------------------------*/
    ui->ZoneBox->blockSignals(true);
    ui->LEDBox->blockSignals(true);
    ui->PerLEDCheck->blockSignals(true);
    ui->ModeSpecificCheck->blockSignals(true);
    ui->RandomCheck->blockSignals(true);
    ui->SpeedSlider->blockSignals(true);
    ui->DirectionBox->blockSignals(true);
    ui->comboBox_modeSpecificColor->blockSignals(true);


    ui->pushButton_AddEffects->blockSignals(true);



    /*
     * Clear all controls initially
     */
    ui->ZoneBox->clear();
    ui->PerLEDCheck->setAutoExclusive(false);
    ui->ModeSpecificCheck->setAutoExclusive(false);
    ui->RandomCheck->setAutoExclusive(false);
    ui->PerLEDCheck->setChecked(false);
    ui->ModeSpecificCheck->setChecked(false);
    ui->RandomCheck->setChecked(false);
    ui->SpeedSlider->setValue(0);
    ui->DirectionBox->clear();
    ui->comboBox_modeSpecificColor->clear();

    /*
     *
     * Disable all controls initially
     *
     * -----------------------------------------------------*/
    ui->ZoneBox->setEnabled(false);
    ui->PerLEDCheck->setEnabled(false);
    ui->ModeSpecificCheck->setEnabled(false);
    ui->RandomCheck->setEnabled(false);
    ui->SpeedSlider->setEnabled(false);
    ui->DirectionBox->setEnabled(false);
    ui->comboBox_modeSpecificColor->setEnabled(false);
    ui->pushButton_AddEffects->setEnabled(false);




    ui->ColorFrame->setVisible(false);
    ui->DeviceViewBox->setPerLED(false);
    ui->pushButtonToggleLEDView->setEnabled(false);

    /*-----------------------------------------------------*\
    | Read selected mode                                    |
    \*-----------------------------------------------------*/
    auto mode = device->GetModeByIdx(ui->ModeBox->currentIndex());

    unsigned int  effective_color_mode      = (selectColorMode < LenovoLegionDaemon::MODE_COLORS_MAX) ? selectColorMode : mode.color_mode;
    bool supports_per_led                   = mode.flags & LenovoLegionDaemon::MODE_FLAG_HAS_PER_LED_COLOR;
    bool supports_mode_specific             = mode.flags & LenovoLegionDaemon::MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    bool supports_random                    = mode.flags & LenovoLegionDaemon::MODE_FLAG_HAS_RANDOM_COLOR;
    bool supports_speed                     = mode.flags & LenovoLegionDaemon::MODE_FLAG_HAS_SPEED;
    bool supports_dir_lr                    = mode.flags & LenovoLegionDaemon::MODE_FLAG_HAS_DIRECTION_LR;
    bool supports_dir_ud                    = mode.flags & LenovoLegionDaemon::MODE_FLAG_HAS_DIRECTION_UD;
    bool supports_spin_lr                    = mode.flags & LenovoLegionDaemon::MODE_FLAG_HAS_DIRECTION_SPINLR;

    bool per_led                = effective_color_mode == LenovoLegionDaemon::MODE_COLORS_PER_LED;
    bool mode_specific          = effective_color_mode == LenovoLegionDaemon::MODE_COLORS_MODE_SPECIFIC;
    bool random                 = effective_color_mode == LenovoLegionDaemon::MODE_COLORS_RANDOM;

    if(supports_speed)
    {
        ui->SpeedSlider->setMinimum(mode.speed_min);
        ui->SpeedSlider->setMaximum(mode.speed_max);

        ui->SpeedSlider->setValue(mode.speed);
        ui->SpeedSlider->setEnabled(true);
        ui->SpeedSlider->blockSignals(false);
    }

    if(supports_dir_lr)
    {
        ui->DirectionBox->addItem(LenovoLegionDaemon::modeDirectionToString(LenovoLegionDaemon::MODE_DIRECTION_LEFT));
        ui->DirectionBox->addItem(LenovoLegionDaemon::modeDirectionToString(LenovoLegionDaemon::MODE_DIRECTION_RIGHT));
    }

    if(supports_dir_ud)
    {
        ui->DirectionBox->addItem(LenovoLegionDaemon::modeDirectionToString(LenovoLegionDaemon::MODE_DIRECTION_UP));
        ui->DirectionBox->addItem(LenovoLegionDaemon::modeDirectionToString(LenovoLegionDaemon::MODE_DIRECTION_DOWN));
    }

    if(supports_spin_lr)
    {
        ui->DirectionBox->addItem(LenovoLegionDaemon::modeDirectionToString(LenovoLegionDaemon::MODE_DIRECTION_SPINLEFT));
        ui->DirectionBox->addItem(LenovoLegionDaemon::modeDirectionToString(LenovoLegionDaemon::MODE_DIRECTION_SPINRIGHT));
    }

    if(supports_dir_lr || supports_dir_ud || supports_spin_lr)
    {
        ui->DirectionBox->setEnabled(true);
    }

    if(supports_per_led)
    {
        ui->PerLEDCheck->setEnabled(true);
        ui->PerLEDCheck->setChecked(per_led);
        ui->PerLEDCheck->setAutoExclusive(true);
        ui->PerLEDCheck->blockSignals(false);
    }

    if(supports_mode_specific)
    {
        ui->ModeSpecificCheck->setEnabled(true);
        ui->ModeSpecificCheck->setChecked(mode_specific);
        ui->ModeSpecificCheck->setAutoExclusive(true);
        ui->ModeSpecificCheck->blockSignals(false);
    }

    if(supports_random)
    {
        ui->RandomCheck->setEnabled(true);
        ui->RandomCheck->setChecked(random);
        ui->RandomCheck->setAutoExclusive(true);
        ui->RandomCheck->blockSignals(false);
    }

    /*-----------------------------------------------------*\
    | Fill in the zone box based on color mode              |
    \*-----------------------------------------------------*/
    switch(effective_color_mode)
    {
        case LenovoLegionDaemon::MODE_COLORS_NONE:
        case LenovoLegionDaemon::MODE_COLORS_RANDOM:
            break;
        case LenovoLegionDaemon::MODE_COLORS_PER_LED:
            ui->ColorFrame->setVisible(true);
            break;
        case LenovoLegionDaemon::MODE_COLORS_MODE_SPECIFIC:
        {
            for(unsigned int i = 0; i < mode.colors.size();++i)
            {
                ui->comboBox_modeSpecificColor->addItem(QString::asprintf("Mode Color %u",i),QVariant(QColor(QRgb(mode.colors.at(i)))));
            }

            ui->comboBox_modeSpecificColor->setEnabled(true);
            ui->comboBox_modeSpecificColor->setCurrentIndex(0);
            on_comboBox_modeSpecificColor_currentIndexChanged(0);
            ui->comboBox_modeSpecificColor->blockSignals(false);
            ui->ColorFrame->setVisible(true);
        }
            break;
    }



    /*
     * Fill in the zone selection box
     */
    const auto& zones = device->GetZones();

    if (zones.size() > 1)
    {
        ui->ZoneBox->addItem(tr("All Zones"));
    }

    for(const auto& zone : zones)
    {
        ui->ZoneBox->addItem(zone.name.c_str());
    }

    ui->ZoneBox->setCurrentIndex(0);
    ui->ZoneBox->setEnabled(true);
    ui->ZoneBox->blockSignals(false);


    /*
     * Device View
     */
    if(mode.flags & LenovoLegionDaemon::MODE_FLAG_HAS_PER_LED_SELECTION)
    {
        ui->LEDBox->setEnabled(true);

        ui->DeviceViewBox->selectLeds([this](){
            QVector<int> indices;
            for (int i = 0; i < ui->LEDBox->count(); ++i) {

                auto led = ui->LEDBox->itemData(i).value<LenovoLegionDaemon::led>();

                for(size_t j = 0; j < device->GetLEDs().size(); ++j)
                {
                    if(device->GetLEDs().at(j).value == led.value)
                    {
                        indices.push_back((int)j);
                    }
                }
            }

            return indices;
        }());
        ui->DeviceViewBox->setPerLED(true);
    }
    else if(mode.flags & LenovoLegionDaemon::MODE_FLAG_HAS_ALL_LED_SELECTION)
    {
        ui->LEDBox->clear();
        ui->LEDBox->setEnabled(true);

        ui->DeviceViewBox->selectLeds([this](){
            QVector<int> indices;

            for(size_t i = 0; i < device->GetLEDs().size(); ++i)
            {
                indices.push_back((int)i);
            }

            return indices;
        }());
    }
    else if(mode.flags & LenovoLegionDaemon::MODE_FLAG_HAS_ALL_KB_LED_SELECTION)
    {
        ui->LEDBox->clear();
        ui->LEDBox->setEnabled(true);

        ui->DeviceViewBox->selectLeds([this](){
            QVector<int> indices;

            for(size_t i = 0; i < device->GetLEDs().size(); ++i)
            {
                if(LenovoLegionDaemon::KeyCodesToName.contains(device->GetLEDs().at(i).value))
                {
                    indices.push_back((int)i);
                }
            }

            return indices;
        }());
    }
    else
    {
        ui->LEDBox->clear();
        ui->LEDBox->setEnabled(false);


        ui->pushButton_AddEffects->setEnabled(true);
        ui->pushButton_AddEffects->blockSignals(false);
    }

    ui->pushButtonToggleLEDView->setEnabled(true);
    ShowDeviceView();
}

void OpenRGBDevicePage::SetDevice(unsigned char red, unsigned char green, unsigned char blue)
{
    current_color.setRgb(red, green, blue);

    /*-----------------------------------------------------*\
    | Update the color UI                                   |
    \*-----------------------------------------------------*/
    colorChanged();
}

void OpenRGBDevicePage::on_SwatchBox_swatchChanged(const QColor color)
{
    /*-----------------------------------------------------*\
    | Store the swatch color to the current color QColor    |
    \*-----------------------------------------------------*/
    current_color = color;

    /*-----------------------------------------------------*\
    | Update the color UI                                   |
    \*-----------------------------------------------------*/
    colorChanged();
}

void OpenRGBDevicePage::on_ColorWheelBox_colorChanged(const QColor color)
{
    /*-----------------------------------------------------*\
    | Store the wheel color to the current color QColor     |
    \*-----------------------------------------------------*/
    current_color = color;

    /*-----------------------------------------------------*\
    | Update the color UI                                   |
    \*-----------------------------------------------------*/
    colorChanged();
}

void OpenRGBDevicePage::on_RedSpinBox_valueChanged(int red)
{
    /*-----------------------------------------------------*\
    | Update the current color QColor red channel           |
    \*-----------------------------------------------------*/
    current_color.setRed(red);

    /*-----------------------------------------------------*\
    | Update the color UI                                   |
    \*-----------------------------------------------------*/
    colorChanged();
}

void OpenRGBDevicePage::on_HueSpinBox_valueChanged(int hue)
{
    /*-----------------------------------------------------*\
    | Read the saturation and value box values              |
    \*-----------------------------------------------------*/
    int sat = current_color.saturation();
    int val = current_color.value();

    /*-----------------------------------------------------*\
    | Update the current color QColor using HSV             |
    \*-----------------------------------------------------*/
    current_color.setHsv(hue, sat, val);

    /*-----------------------------------------------------*\
    | Update the color UI                                   |
    \*-----------------------------------------------------*/
    colorChanged();
}

void OpenRGBDevicePage::on_GreenSpinBox_valueChanged(int green)
{
    /*-----------------------------------------------------*\
    | Update the current color QColor green channel         |
    \*-----------------------------------------------------*/
    current_color.setGreen(green);

    /*-----------------------------------------------------*\
    | Update the color UI                                   |
    \*-----------------------------------------------------*/
    colorChanged();
}

void OpenRGBDevicePage::on_SatSpinBox_valueChanged(int sat)
{
    /*-----------------------------------------------------*\
    | Read the hue and value box values                     |
    \*-----------------------------------------------------*/
    int hue = current_color.hue();
    int val = current_color.value();

    /*-----------------------------------------------------*\
    | Update the current color QColor using HSV             |
    \*-----------------------------------------------------*/
    current_color.setHsv(hue, sat, val);

    /*-----------------------------------------------------*\
    | Update the color UI                                   |
    \*-----------------------------------------------------*/
    colorChanged();
}

void OpenRGBDevicePage::on_BlueSpinBox_valueChanged(int blue)
{
    /*-----------------------------------------------------*\
    | Update the current color QColor blue channel          |
    \*-----------------------------------------------------*/
    current_color.setBlue(blue);

    /*-----------------------------------------------------*\
    | Update the color UI                                   |
    \*-----------------------------------------------------*/
    colorChanged();
}

void OpenRGBDevicePage::on_ValSpinBox_valueChanged(int val)
{
    /*-----------------------------------------------------*\
    | Read the hue and saturation box values                |
    \*-----------------------------------------------------*/
    int hue = current_color.hue();
    int sat = current_color.saturation();

    /*-----------------------------------------------------*\
    | Update the current color QColor using HSV             |
    \*-----------------------------------------------------*/
    current_color.setHsv(hue, sat, val);

    /*-----------------------------------------------------*\
    | Update the color UI                                   |
    \*-----------------------------------------------------*/
    colorChanged();
}

void OpenRGBDevicePage::on_HexLineEdit_textChanged(const QString &arg1)
{
    /*-----------------------------------------------------*\
    | Make an editable copy of the string                   |
    \*-----------------------------------------------------*/
    QString temp = arg1;

    /*-----------------------------------------------------*\
    | Remove # character so that #XXXXXX color codes are    |
    | acceptable.  0xXXXXXX codes are already accepted by   |
    | toInt().  Convert into an RGBColor.  Mask off the     |
    | unused bits.                                          |
    \*-----------------------------------------------------*/
    LenovoLegionDaemon::RGBColor color = (LenovoLegionDaemon::RGBColor)(0x00FFFFFF & temp.replace("#", "").toInt(NULL, 16));

    /*-----------------------------------------------------*\
    | Store new color into the current color QColor         |
    | Because RGBColor stores color in BGR format, we have  |
    | to reverse the R and B channels if the hex format is  |
    | RGB.                                                  |
    \*-----------------------------------------------------*/
    if(HexFormatRGB)
    {
        current_color.setRed(RGBGetBValue(color));
        current_color.setGreen(RGBGetGValue(color));
        current_color.setBlue(RGBGetRValue(color));
    }
    else
    {
        current_color.setRed(RGBGetRValue(color));
        current_color.setGreen(RGBGetGValue(color));
        current_color.setBlue(RGBGetBValue(color));
    }

    /*-----------------------------------------------------*\
    | Update the color UI, but set the UpdateHex flag to    |
    | false so the hex edit box isn't updated while the user|
    | is in the middle of typing a value.                   |
    \*-----------------------------------------------------*/
    UpdateHex = false;
    colorChanged();
    UpdateHex = true;
}

void OpenRGBDevicePage::on_DeviceViewBox_selectionChanged(QVector<int> indices)
{
    ui->LEDBox->blockSignals(true);
    ui->pushButton_AddEffects->blockSignals(true);

    ui->LEDBox->setEnabled(false);
    ui->pushButton_AddEffects->setEnabled(false);

    ui->LEDBox->clear();

    for (const auto& led : indices)
    {
        ui->LEDBox->addItem(device->GetLEDName(led).c_str(),QVariant::fromValue(device->GetLEDs().at(led)));
    }

    if(ui->LEDBox->count() > 0)
    {
        ui->LEDBox->setEnabled(true);
        ui->LEDBox->blockSignals(false);
        ui->pushButton_AddEffects->setEnabled(true);
        ui->pushButton_AddEffects->blockSignals(false);
    }

    if(ui->PerLEDCheck->isChecked())
    {
        ui->DeviceViewBox->setSelectionColor(ToRGBColor(current_color.red(), current_color.green(), current_color.blue()));
    }
}

void OpenRGBDevicePage::ShowDeviceView()
{
    /*-----------------------------------------------------*\
     * Show device view                                     *
     * -----------------------------------------------------*/
    ui->DeviceViewBoxFrame->show();
}

void OpenRGBDevicePage::HideDeviceView()
{
    /*-----------------------------------------------------*\
    | Hide device view                                      |
    \*-----------------------------------------------------*/
    ui->DeviceViewBoxFrame->hide();
}

void OpenRGBDevicePage::UpdateEffectUi(unsigned int selectEffectIndx, unsigned int selectModeColorIdx)
{
    ui->listWidgetEffects->blockSignals(true);
    ui->SelectedEffectsBox->blockSignals(true);
    ui->LEDEffectsBox->blockSignals(true);
    ui->DirectionEffectsBox->blockSignals(true);
    ui->ModeEffectsBox->blockSignals(true);
    ui->SpeedEffectsComboBox->blockSignals(true);
    ui->comboBox_EffectsColors->blockSignals(true);
    ui->RandomEffectsCheck->blockSignals(true);
    ui->ModeSpecificEffectsCheck->blockSignals(true);
    ui->PerLEDEffectsCheck->blockSignals(true);
    ui->pushButton_EffectDelete->blockSignals(true);
    ui->pushButton_EffectsUnselect->blockSignals(true);



    ui->listWidgetEffects->setEnabled(false);
    ui->SelectedEffectsBox->setEnabled(false);
    ui->LEDEffectsBox->setEnabled(false);
    ui->DirectionEffectsBox->setEnabled(false);
    ui->ModeEffectsBox->setEnabled(false);
    ui->SpeedEffectsComboBox->setEnabled(false);
    ui->comboBox_EffectsColors->setEnabled(false);
    ui->RandomEffectsCheck->setEnabled(false);
    ui->ModeSpecificEffectsCheck->setEnabled(false);
    ui->PerLEDEffectsCheck->setEnabled(false);
    ui->pushButton_EffectDelete->setEnabled(false);
    ui->pushButton_EffectsUnselect->setEnabled(false);


    ui->listWidgetEffects->clear();
    ui->SelectedEffectsBox->clear();
    ui->LEDEffectsBox->clear();
    ui->DirectionEffectsBox->clear();
    ui->ModeEffectsBox->clear();
    ui->SpeedEffectsComboBox->clear();
    ui->comboBox_EffectsColors->clear();
    ui->RandomEffectsCheck->setAutoExclusive(false);
    ui->ModeSpecificEffectsCheck->setAutoExclusive(false);
    ui->PerLEDEffectsCheck->setAutoExclusive(false);
    ui->RandomEffectsCheck->setChecked(false);
    ui->ModeSpecificEffectsCheck->setChecked(false);
    ui->PerLEDEffectsCheck->setChecked(false);


    const auto& effects = device->GetEffects();


    for (size_t i = 0; i < effects.size(); ++i)
    {
        ui->listWidgetEffects->addItem(new QListWidgetItem(QString::number(i) + " - " + device->GetModeByModeValue(effects.at(i).m_mode).name.c_str()));
    }

    if(selectEffectIndx < effects.size())
    {
        auto effect = device->GetEffect(selectEffectIndx);

        if(effect.m_color_mode == LenovoLegionDaemon::MODE_COLORS_RANDOM)
        {
            ui->RandomEffectsCheck->setChecked(true);
        }

        if(effect.m_color_mode == LenovoLegionDaemon::MODE_COLORS_PER_LED)
        {
            ui->PerLEDEffectsCheck->setChecked(true);
        }

        if(effect.m_color_mode == LenovoLegionDaemon::MODE_COLORS_MODE_SPECIFIC)
        {
            ui->ModeSpecificEffectsCheck->setChecked(true);
        }

        ui->SelectedEffectsBox->addItem(QString::number(selectEffectIndx) + " - " + device->GetModeByModeValue(effect.m_mode).name.c_str(),QVariant::fromValue(selectEffectIndx));

        for (const auto& led_effect : effect.m_leds)
        {
            ui->LEDEffectsBox->addItem(QString(led_effect.name.c_str()));
        }

        ui->DirectionEffectsBox->addItem(LenovoLegionDaemon::modeDirectionToString(LenovoLegionDaemon::valueToModeDirection(effect.m_direction)));


        ui->ModeEffectsBox->addItem(device->GetModeByModeValue(effect.m_mode).name.c_str());

        ui->SpeedEffectsComboBox->addItem(QString::number(effect.m_speed));

        for (const auto& color : effect.m_colors)
        {
            ui->comboBox_EffectsColors->addItem(QString().asprintf("R:%02X G:%02X B:%02X", RGBGetRValue(color),RGBGetGValue(color),RGBGetBValue(color)));
        }

        ui->DeviceViewBox->markLeds([&effect,this,selectModeColorIdx](){
            QMap<int,QColor> indices;
            for (const auto& led_effect : effect.m_leds)
            {
                auto ledsIdx = device->GetLedsIndexesByDeviceSpecificValue(led_effect.value);

                for (auto idx : ledsIdx) {

                    if(selectModeColorIdx < effect.m_colors.size())
                    {
                        indices[idx] = QColor::fromRgb(RGBGetRValue(effect.m_colors[selectModeColorIdx]),RGBGetGValue(effect.m_colors[selectModeColorIdx]),RGBGetBValue(effect.m_colors[selectModeColorIdx]));
                    }
                    else
                    {
                        if(effect.m_colors.size() > 0)
                        {
                            indices[idx] = QColor::fromRgb(RGBGetRValue(effect.m_colors[0]),RGBGetGValue(effect.m_colors[0]),RGBGetBValue(effect.m_colors[0]));
                        }
                        else
                        {
                            indices[idx] = QColor::fromRgb(0,0,0);
                        }
                    }
                }
            }
            return indices;
        }());

        ui->pushButton_EffectDelete->setEnabled(true);
        ui->pushButton_EffectsUnselect->setEnabled(true);
        ui->pushButton_EffectDelete->blockSignals(false);
        ui->pushButton_EffectsUnselect->blockSignals(false);
    }
    else
    {
        ui->DeviceViewBox->markLeds({});
    }

    /*
     * Enable that have data
     */
    if(ui->comboBox_EffectsColors->count() > 0 )
    {
        ui->comboBox_EffectsColors->setCurrentIndex(selectModeColorIdx < static_cast<unsigned int>(ui->comboBox_EffectsColors->count()) ? selectModeColorIdx : 0);
        ui->comboBox_EffectsColors->setEnabled(true);
        ui->comboBox_EffectsColors->blockSignals(false);
    }

    if(ui->listWidgetEffects->count() > 0 )
    {
        ui->listWidgetEffects->setCurrentRow(selectEffectIndx < static_cast<unsigned int>(ui->listWidgetEffects->count()) ? selectEffectIndx : -1);
        ui->listWidgetEffects->setEnabled(true);
        ui->listWidgetEffects->blockSignals(false);
    }

    if(ui->LEDEffectsBox->count() > 0 )
    {
        ui->LEDEffectsBox->setEnabled(true);
    }
}

void OpenRGBDevicePage::on_pushButton_AddEffects_clicked()
{
    device->AddEffect([this](){
        LenovoLegionDaemon::led_group_effect effect;

        effect.m_mode       = device->GetModeByIdx(ui->ModeBox->currentIndex()).value;
        effect.m_speed      = ui->SpeedSlider->isEnabled() ? ui->SpeedSlider->value() : 0;
        effect.m_direction  = LenovoLegionDaemon::stringToModeDirection(ui->DirectionBox->currentText());
        effect.m_color_mode = LenovoLegionDaemon::MODE_COLORS_NONE;

        if(ui->PerLEDCheck->isChecked())
        {
            effect.m_color_mode = LenovoLegionDaemon::MODE_COLORS_PER_LED;
            effect.m_colors.push_back(ToRGBColor(current_color.red(), current_color.green(), current_color.blue()));
        }

        if(ui->ModeSpecificCheck->isChecked())
        {
            effect.m_color_mode = LenovoLegionDaemon::MODE_COLORS_MODE_SPECIFIC;

            for (int i = 0; i < ui->comboBox_modeSpecificColor->count(); ++i)
            {
                QColor color = ui->comboBox_modeSpecificColor->itemData(i).value<QColor>();
                effect.m_colors.push_back(ToRGBColor(color.red(), color.green(), color.blue()));
            }
        }

        if(ui->RandomCheck->isChecked())
        {
            effect.m_color_mode = LenovoLegionDaemon::MODE_COLORS_RANDOM;
        }

        for (int i = 0; i < ui->LEDBox->count(); ++i)
        {
            effect.m_leds.push_back(ui->LEDBox->itemData(i).value<LenovoLegionDaemon::led>());
        }

        return effect;
    }());

    device->ApplyPendingChanges();
    UpdateEffectUi();
}

void OpenRGBDevicePage::colorChanged()
{
    updateColorUi();


    /*-----------------------------------------------------------------*\
    | OpenRGB's RGBColor is stored differently than Qt's qrgb type,     |
    | so casting between them doesn't work                              |
    \*-----------------------------------------------------------------*/
    if(ui->PerLEDCheck->isChecked())
    {
        ui->DeviceViewBox->setSelectionColor(ToRGBColor(current_color.red(), current_color.green(), current_color.blue()));
    }

    if(ui->ModeSpecificCheck->isChecked())
    {
        ui->comboBox_modeSpecificColor->setItemData(ui->comboBox_modeSpecificColor->currentIndex(), QVariant(current_color));
    }
}

void OpenRGBDevicePage::updateColorUi()
{
    /*-----------------------------------------------------*\
    | Update colorwheel                                     |
    \*-----------------------------------------------------*/
    ui->ColorWheelBox->blockSignals(true);
    ui->ColorWheelBox->setColor(current_color);
    ui->ColorWheelBox->blockSignals(false);

    /*-----------------------------------------------------*\
    | Update RGB spinboxes                                  |
    \*-----------------------------------------------------*/
    ui->RedSpinBox->blockSignals(true);
    ui->RedSpinBox->setValue(current_color.red());
    ui->RedSpinBox->blockSignals(false);

    ui->GreenSpinBox->blockSignals(true);
    ui->GreenSpinBox->setValue(current_color.green());
    ui->GreenSpinBox->blockSignals(false);

    ui->BlueSpinBox->blockSignals(true);
    ui->BlueSpinBox->setValue(current_color.blue());
    ui->BlueSpinBox->blockSignals(false);

    /*-----------------------------------------------------*\
    | Update HSV spinboxes                                  |
    \*-----------------------------------------------------*/
    ui->HueSpinBox->blockSignals(true);
    ui->HueSpinBox->setValue(current_color.hue());
    ui->HueSpinBox->blockSignals(false);

    ui->SatSpinBox->blockSignals(true);
    ui->SatSpinBox->setValue(current_color.saturation());
    ui->SatSpinBox->blockSignals(false);

    ui->ValSpinBox->blockSignals(true);
    ui->ValSpinBox->setValue(current_color.value());
    ui->ValSpinBox->blockSignals(false);

    /*-----------------------------------------------------*\
    | Update Hex edit box                                   |
    \*-----------------------------------------------------*/
    if(UpdateHex)
    {
        LenovoLegionDaemon::RGBColor color = (0x00FFFFFF & current_color.rgb());

        /*-------------------------------------------------*\
        | If the hex format is BGR, swap R and B before     |
        | displaying as hex                                 |
        \*-------------------------------------------------*/
        if(!HexFormatRGB)
        {
            color = RGBGetRValue(color) << 16
                  | RGBGetGValue(color) << 8
                  | RGBGetBValue(color);
        }

        ui->HexLineEdit->blockSignals(true);
        ui->HexLineEdit->setText(QString().asprintf("%06X", color));
        ui->HexLineEdit->blockSignals(false);
    }
}

void OpenRGBDevicePage::on_ProfileBox_currentIndexChanged(int index)
{
    /*-----------------------------------------------------*\
    | Change device profile                                    |
    \*-----------------------------------------------------*/
    device->SetProfile(ui->ProfileBox->itemData(index).toUInt());

    device->ApplyPendingChanges();

    UpdateInterface();
}

void OpenRGBDevicePage::on_pushButtonToggleLEDView_clicked()
{
    if(!ui->DeviceViewBoxFrame->isHidden())
    {
        HideDeviceView();
    }
    else
    {
        ShowDeviceView();
    }
}

void OpenRGBDevicePage::on_listWidgetEffects_currentRowChanged(int currentRow)
{
    UpdateEffectUi(currentRow);
}

void OpenRGBDevicePage::on_pushButton_EffectsUnselect_clicked()
{
    ui->listWidgetEffects->clearSelection();

    UpdateEffectUi();
}

void OpenRGBDevicePage::on_pushButton_EffectsClearAll_clicked()
{
    device->ClearEffects();
    device->ApplyPendingChanges();
    UpdateEffectUi();
}

void OpenRGBDevicePage::on_ModeBox_currentIndexChanged(int)
{
    ui->DeviceViewBox->clearSelection();
    ui->DeviceViewBox->setSelectionColor(ToRGBColor(0,0,0));

    UpdateModeUi();
}

void OpenRGBDevicePage::on_PerLEDCheck_clicked()
{
    UpdateModeUi(LenovoLegionDaemon::MODE_COLORS_PER_LED);
}

void OpenRGBDevicePage::on_ModeSpecificCheck_clicked()
{
    UpdateModeUi(LenovoLegionDaemon::MODE_COLORS_MODE_SPECIFIC);
}

void OpenRGBDevicePage::on_RandomCheck_clicked()
{
    UpdateModeUi(LenovoLegionDaemon::MODE_FLAG_HAS_RANDOM_COLOR);
}

void OpenRGBDevicePage::on_comboBox_modeSpecificColor_currentIndexChanged(int index)
{
    current_color = ui->comboBox_modeSpecificColor->itemData(index).value<QColor>();
    updateColorUi();
}

void OpenRGBDevicePage::on_pushButton_EffectDelete_clicked()
{
    if(ui->SelectedEffectsBox->count() > 0)
    {
        device->RemoveEffect(ui->SelectedEffectsBox->itemData(0).value<int>());
        device->ApplyPendingChanges();
        UpdateEffectUi();
    }
}

void OpenRGBDevicePage::on_comboBox_EffectsColors_currentIndexChanged(int index)
{
    UpdateEffectUi(ui->listWidgetEffects->currentRow(), index);
}

void OpenRGBDevicePage::on_pushButton_EffectsDefault_clicked()
{
    device->ResetEffectsToDefault();
    device->ApplyPendingChanges();
    UpdateEffectUi();
}

}
