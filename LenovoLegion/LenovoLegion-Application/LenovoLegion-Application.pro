TEMPLATE = app
TARGET = $${APPLICATION_NAME}

QT       += core gui widgets network charts

CONFIG += c++20

DESTDIR = $${DESTINATION_BIN_PATH}

SOURCES +=               \
        AboutWindow.cpp \
        Application.cpp  \
        BateryStatus.cpp \
        BateryStatusDataProvider.cpp \
        CPUControl.cpp \
        CPUControlDataProvider.cpp \
        CPUFrequency.cpp \
        CPUFrequencyControl.cpp \
        CPUFrequencyControlDataProvider.cpp \
        DaemonDataProvider.cpp \
        FanControl.cpp \
        FanControlDataProvider.cpp \
        HWMonitor.cpp \
        HWMonitoring.cpp \
        HWMonitoringDataProvider.cpp \
        MainWindow.cpp   \
        PowerControl.cpp \
        PowerControlDataProvider.cpp \
        PowerProfileControl.cpp \
        PowerProfileControlDataProvider.cpp \
        ProtocolProcessor.cpp \
        ProtocolProcessorBase.cpp \
        ProtocolProcessorNotifier.cpp \
        Settings.cpp \
        ThreadControl.cpp \
        ThreadFrequency.cpp \
        ThreadFrequencyControl.cpp \
        main.cpp            

HEADERS +=          \
        AboutWindow.h \
        Application.h \
        BateryStatus.h \
        BateryStatusDataProvider.h \
        CPUControl.h \
        CPUControlDataProvider.h \
        CPUFrequency.h \
        CPUFrequencyControl.h \
        CPUFrequencyControlDataProvider.h \
        DaemonDataProvider.h \
        DataProvider.h \
        FanControl.h \
        FanControlDataProvider.h \
        HWMonitor.h \
        HWMonitoring.h \
        HWMonitoringDataProvider.h \
        MainWindow.h  \
        PowerControl.h \
        PowerControlDataProvider.h \
        PowerProfileControl.h \
        PowerProfileControlDataProvider.h \
        ProtocolProcessor.h \
        ProtocolProcessorBase.h \
        ProtocolProcessorNotifier.h \
        Settings.h \
        ThreadControl.h \
        ThreadFrequency.h \
        ThreadFrequencyControl.h \
        WidgetMessage.h

HEADERS += \
        ../LenovoLegion-Daemon/ProtocolParser.h

SOURCES += \
        ../LenovoLegion-Daemon/ProtocolParser.cpp

FORMS +=           \
    AboutWindow.ui \
    BateryStatus.ui \
    CPUControl.ui \
    CPUFrequency.ui \
    CPUFrequencyControl.ui \
    FanControl.ui \
    HWMonitor.ui \
    HWMonitoring.ui \
    MainWindow.ui \
    PowerControl.ui \
    PowerProfileControl.ui \
    ThreadControl.ui \
    ThreadFrequency.ui \
    ThreadFrequencyControl.ui

LIBS += -l$${PROJECT_LIBS_NAME}

RESOURCES += \
    Resources.qrc

OTHER_FILES += \
    LenovoLegion.desktop

QMAKE_POST_LINK += $$quote($${SYSTEM_CP} $${PROJECT_ROOT_PATH}/LenovoLegion-Application/LenovoLegionIco.png $${DESTINATION_PATH}$$escape_expand(\\n\\t))
