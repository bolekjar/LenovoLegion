#include "ToolBarKeyboardWidget.h"
#include "ui_ToolBarKeyboardWidget.h"

#include "Utils.h"
#include "WidgetMessage.h"

#include "Core/LoggerHolder.h"


#include "RGBController.h"
#include "OpenRGBDevicePage.h"


namespace LenovoLegionGui {

ToolBarKeyboardWidget::ToolBarKeyboardWidget(DataProvider* dataProvider,QWidget *parent)
    : ToolBarWidget(dataProvider,parent)
    , ui(new Ui::ToolBarKeyboardWidget)
{
    ui->setupUi(this);

    /*
     * Initialize actions map
     */
    m_defaultActionsMap["add"].push_back([this]() {
        try {
            ui->verticalLayout_ToolBarKeyboard->addWidget(new OpenRGBDevicePage(new RGBController(m_dataProvider),this));
        }
        catch(RGBController::exception_T &ex)
        {
            LOG_D(QString("ToolBarKeyboardWidget: Failed to initialize OpenRGBDevicePage: ").append(ex.what()));
        }
    });


    m_defaultActionsMap["remove"].push_back([this]() {
        Utils::Layout::removeAllLayoutWidgets(*ui->verticalLayout_ToolBarKeyboard);
    });

    m_defaultActionsMap["refresh"].push_back([this]() {
        Utils::Task::executeTasks(m_defaultActionsMap["remove"]);
        Utils::Task::executeTasks(m_defaultActionsMap["add"]);
    });

    Utils::Task::insertTasksBack(m_asyncTasks,m_defaultActionsMap["add"]);
}

void ToolBarKeyboardWidget::timerEvent(QTimerEvent *event)
{
    ToolBarWidget::timerEvent(event);
}

void ToolBarKeyboardWidget::dataProviderEvent(const legion::messages::Notification &notification)
{
    if(notification.has_action())
    {
        if(notification.action() == legion::messages::Notification::SPECIAL_KEY_PRESSED && notification.has_special_key())
        {
            switch (notification.special_key()) {
                case legion::messages::Notification::SPECTRUMBACKLIGHTOFF:
                case legion::messages::Notification::SPECTRUMBACKLIGHT1:
                case legion::messages::Notification::SPECTRUMBACKLIGHT2:
                case legion::messages::Notification::SPECTRUMBACKLIGHT3:
                case legion::messages::Notification::SPECTRUMPRESET1:
                case legion::messages::Notification::SPECTRUMPRESET2:
                case legion::messages::Notification::SPECTRUMPRESET3:
                case legion::messages::Notification::SPECTRUMPRESET4:
                case legion::messages::Notification::SPECTRUMPRESET5:
                case legion::messages::Notification::SPECTRUMPRESET6:
                    Utils::Task::executeTasks(m_defaultActionsMap["refresh"]);
                    break;
                default:
                    break;
            }
        }
    }
}

void ToolBarKeyboardWidget::cleanup()
{

    ToolBarWidget::cleanup();
}

ToolBarKeyboardWidget::~ToolBarKeyboardWidget()
{
    Utils::Task::executeTasks(m_defaultActionsMap["remove"]);
    delete ui;
}

void ToolBarKeyboardWidget::widgetEvent(const WidgetMessage &)
{
}


}
