#include "ToolBarKeyboardWidget.h"
#include "ui_ToolBarKeyboardWidget.h"

#include "Utils.h"
#include "WidgetMessage.h"

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
        ui->verticalLayout_ToolBarKeyboard->addWidget(new OpenRGBDevicePage(new RGBController(m_dataProvider),this));
    });


    m_defaultActionsMap["remove"].push_back([]() {
        // Keyboard settings cleanup logic
    });

    m_defaultActionsMap["refresh"].push_back([]() {
        // Keyboard settings refresh logic
    });

    Utils::Task::insertTasksBack(m_asyncTasks,m_defaultActionsMap["add"]);
}

void ToolBarKeyboardWidget::timerEvent(QTimerEvent *event)
{
    ToolBarWidget::timerEvent(event);
}

void ToolBarKeyboardWidget::dataProviderEvent(const legion::messages::Notification &)
{}

void ToolBarKeyboardWidget::cleanup()
{
    ToolBarWidget::cleanup();
}

ToolBarKeyboardWidget::~ToolBarKeyboardWidget()
{
    delete ui;
}

void ToolBarKeyboardWidget::widgetEvent(const WidgetMessage &event)
{
    // Handle widget events if needed
    Q_UNUSED(event);
}


}
