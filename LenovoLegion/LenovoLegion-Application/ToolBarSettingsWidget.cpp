#include "ToolBarSettingsWidget.h"
#include "ui_ToolBarSettingsWidget.h"

#include "Settings.h"

#include "Core/LoggerHolder.h"

#include "DataProviderManager.h"
#include "Utils.h"
#include "WidgetMessage.h"



#include "../LenovoLegion-Daemon/DataProviderDaemonSettings.h"
#include "../LenovoLegion-PrepareBuild/DaemonSettings.pb.h"


#include <QMessageBox>
#include <QPixmap>


namespace LenovoLegionGui {


ToolBarSettingsWidget::ToolBarSettingsWidget(DataProvider* dataProvider,QWidget *parent)
    : ToolBarWidget(dataProvider,parent)
    , ui(new Ui::ToolBarSettingsWidget)
{

    ui->setupUi(this);

    /*
     * Initialize actions map
     */

    m_defaultActionsMap["add"].push_back([]() {
        // Settings initialization
        LOG_D("ToolBarSettingsWidget initialized");
    });


    m_defaultActionsMap["remove"].push_back([]() {
        // Settings cleanup if needed
    });

    m_defaultActionsMap["refresh"].push_back([]() {
        // Refresh settings if needed
    });

    // Load settings from file
    loadSettings();

    // Connect signals
    connectSignals();

    Utils::Task::insertTasksBack(m_asyncTasks,m_defaultActionsMap["add"]);
}

void ToolBarSettingsWidget::timerEvent(QTimerEvent *event)
{
    ToolBarWidget::timerEvent(event);
}

void ToolBarSettingsWidget::dataProviderEvent(const legion::messages::Notification &event)
{
    // Settings widget doesn't need to react to most notifications
    Q_UNUSED(event);
}

void ToolBarSettingsWidget::cleanup()
{
    ToolBarWidget::cleanup();
}

ToolBarSettingsWidget::~ToolBarSettingsWidget()
{
    delete ui;
}

void ToolBarSettingsWidget::widgetEvent(const WidgetMessage &)
{}

void ToolBarSettingsWidget::onResetToDefaults()
{
    QMessageBox msgBox(this);
    QPixmap dialogIcon(":/images/icons/dialog.png");
    msgBox.setIconPixmap(dialogIcon.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    msgBox.setWindowTitle("Reset to Defaults");
    msgBox.setText("Are you sure you want to reset all settings to their default values?");
    msgBox.setInformativeText("This action cannot be undone.");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    if (msgBox.exec() == QMessageBox::Yes) {
        resetAllSettingsToDefaults();
        LOG_D("All settings reset to defaults");
    }
}

void ToolBarSettingsWidget::resetAllSettingsToDefaults()
{
    ApplicationSettings::instance()->resetToDefaults();
    loadSettings();
    LOG_D("Settings reset to default values");
}

void ToolBarSettingsWidget::loadSettings()
{
    bool value;
    ApplicationSettings* settings = ApplicationSettings::instance();
    
    // Load application settings from INI
    settings->loadStartMinimized(value);
    ui->checkBox_StartMinimized->setChecked(value);
    
    settings->loadMinimizeToTray(value);
    ui->checkBox_MinimizeToTray->setChecked(value);
    
    settings->loadAppDebugLogging(value);
    ui->checkBox_AppDebugLogging->setChecked(value);
    
    // Load daemon settings from daemon (not from app INI)
    loadDaemonSettings();
    
    LOG_D("Application settings loaded from file");
}

void ToolBarSettingsWidget::saveApplicationSettings()
{
    ApplicationSettings* settings = ApplicationSettings::instance();
    settings->saveStartMinimized(ui->checkBox_StartMinimized->isChecked())
            .saveMinimizeToTray(ui->checkBox_MinimizeToTray->isChecked())
            .saveAppDebugLogging(ui->checkBox_AppDebugLogging->isChecked());
    
    LOG_D("Application settings saved to file");
}

void ToolBarSettingsWidget::connectSignals()
{
    // Connect application settings checkboxes (saved to app INI)
    connect(ui->checkBox_StartMinimized, &QCheckBox::checkStateChanged, this, &ToolBarSettingsWidget::onApplicationSettingChanged);
    connect(ui->checkBox_MinimizeToTray, &QCheckBox::checkStateChanged, this, &ToolBarSettingsWidget::onApplicationSettingChanged);
    connect(ui->checkBox_AppDebugLogging, &QCheckBox::checkStateChanged, this, &ToolBarSettingsWidget::onApplicationSettingChanged);
    
    // Connect daemon settings checkboxes (sent to daemon only)
    connect(ui->checkBox_ApplySavedSettingsOnStart, &QCheckBox::checkStateChanged, this, &ToolBarSettingsWidget::onDaemonSettingChanged);
    connect(ui->checkBox_SaveSettingsOnDaemonExit, &QCheckBox::checkStateChanged, this, &ToolBarSettingsWidget::onDaemonSettingChanged);
    connect(ui->checkBox_DebugLogging, &QCheckBox::checkStateChanged, this, &ToolBarSettingsWidget::onDaemonSettingChanged);
    
    // Connect Reset to Defaults button
    connect(ui->pushButton_ResetDefaults, &QPushButton::clicked, this, &ToolBarSettingsWidget::onResetToDefaults);
    
    // Connect daemon buttons
    connect(ui->pushButton_SaveCurrentConfiguration, &QPushButton::clicked, this, &ToolBarSettingsWidget::onSaveCurrentConfiguration);
    connect(ui->pushButton_ReconnectDaemon, &QPushButton::clicked, this, &ToolBarSettingsWidget::onReconnectDaemon);
}

void ToolBarSettingsWidget::onApplicationSettingChanged()
{
    saveApplicationSettings();
}

void ToolBarSettingsWidget::onDaemonSettingChanged()
{
    sendDaemonSettingsToDaemon();
}

void ToolBarSettingsWidget::onReconnectDaemon()
{
    LOG_D("Reconnect to daemon button clicked");
    
    // Find DataProviderManager in parent hierarchy
    DataProviderManager* manager = nullptr;
    QWidget* parent = parentWidget();
    
    while (parent) {
        manager = parent->findChild<DataProviderManager*>();
        if (manager) {
            break;
        }
        parent = parent->parentWidget();
    }
    
    if (manager) {
        manager->reconnectToDaemon();
        LOG_D("Daemon reconnection initiated");
    } else {
        LOG_W("Could not find DataProviderManager to reconnect");
    }
}

void ToolBarSettingsWidget::loadDaemonSettings()
{
    legion::messages::DaemonSettings settings = m_dataProvider->getDataMessage<legion::messages::DaemonSettings>(
            LenovoLegionDaemon::DataProviderDaemonSettings::dataType);
        
    // Update UI from daemon settings
    ui->checkBox_ApplySavedSettingsOnStart->setChecked(settings.apply_settings_on_start());
    ui->checkBox_SaveSettingsOnDaemonExit->setChecked(settings.save_settings_on_exit());
    ui->checkBox_DebugLogging->setChecked(settings.debug_logging());

    LOG_D("Daemon settings loaded from daemon");
}

void ToolBarSettingsWidget::sendDaemonSettingsToDaemon()
{
    legion::messages::DaemonSettings settings;
        
    // Set daemon configuration from UI
    settings.set_apply_settings_on_start(ui->checkBox_ApplySavedSettingsOnStart->isChecked());
    settings.set_save_settings_on_exit(ui->checkBox_SaveSettingsOnDaemonExit->isChecked());
    settings.set_debug_logging(ui->checkBox_DebugLogging->isChecked());
        
    m_dataProvider->setDataMessage(LenovoLegionDaemon::DataProviderDaemonSettings::dataType, settings);

    LOG_D("Daemon settings sent to daemon");
}

void ToolBarSettingsWidget::onSaveCurrentConfiguration()
{
    LOG_D("Save Current Configuration button clicked");
    
    QMessageBox msgBox(this);
    QPixmap dialogIcon(":/images/icons/dialog.png");
    msgBox.setIconPixmap(dialogIcon.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    msgBox.setWindowTitle("Save Configuration");
    msgBox.setText("Save the current hardware configuration?");
    msgBox.setInformativeText("This will save all current hardware settings (power profile, CPU settings, fan curves, etc.) to the daemon's configuration file.");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);

    if (msgBox.exec() == QMessageBox::Yes) {
        LOG_D("User confirmed: saving current configuration");
        
        // Send save_now command to daemon
        legion::messages::DaemonSettings settings;
        
        // Get current settings from UI
        settings.set_apply_settings_on_start(ui->checkBox_ApplySavedSettingsOnStart->isChecked());
        settings.set_save_settings_on_exit(ui->checkBox_SaveSettingsOnDaemonExit->isChecked());
        settings.set_debug_logging(ui->checkBox_DebugLogging->isChecked());
        
        // Set the save_now command flag
        settings.set_save_now(true);
        
        // Send to daemon
        m_dataProvider->setDataMessage(LenovoLegionDaemon::DataProviderDaemonSettings::dataType, settings);
        
        LOG_D("Save current configuration command sent to daemon");

        QMessageBox msgBox(this);
        QPixmap dialogIcon(":/images/icons/info.png");
        msgBox.setIconPixmap(dialogIcon.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        msgBox.setWindowTitle("Save Configuration");
        msgBox.setText("Current configuration has been saved to the daemon's configuration file.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    } else {
        LOG_D("User cancelled configuration save");
    }
}


}
