TEMPLATE = app
TARGET = $${APPLICATION_NAME2}

QT       += core network

CONFIG += c++20 cmdline

DESTDIR = $${DESTINATION_BIN_PATH}

SOURCES +=  \
        Application.cpp \
        ProtocolParser.cpp \
        ProtocolProcessor.cpp \
        ProtocolProcessorBase.cpp \
        ProtocolProcessorNotifier.cpp \
        SysFsDataProvider.cpp \
        SysFsDriver.cpp \
        SysFsDriverACPIPlatformProfile.cpp \
        SysFsDriverCPU.cpp \
        SysFsDriverCPUAtom.cpp \
        SysFsDriverCPUCore.cpp \
        SysFsDriverCPUXList.cpp \
        SysFsDriverIntelPowercapRapl.cpp \
        SysFsDriverLenovo.cpp \
        SysFsDriverManager.cpp \
        SysFsDriverPowerSuplyBattery0.cpp \
        Settings.cpp \
        main.cpp

HEADERS += \
    Application.h \
    ControlData.h \
    ControlDataProvider.h \
    Message.h \
    ProtocolParser.h \
    ProtocolProcessor.h \
    ProtocolProcessorBase.h \
    ProtocolProcessorNotifier.h \
    SysFsDataProvider.h \
    Settings.h \
    SysFsDriver.h \
    SysFsDriverACPIPlatformProfile.h \
    SysFsDriverCPU.h \
    SysFsDriverCPUAtom.h \
    SysFsDriverCPUCore.h \
    SysFsDriverCPUXList.h \
    SysFsDriverIntelPowercapRapl.h \
    SysFsDriverLenovo.h \
    SysFsDriverManager.h \
    SysFsDriverPowerSuplyBattery0.h


LIBS += -l$${PROJECT_LIBS_NAME} -ludev

DISTFILES +=  \
    lenovo-legion-daemon.service
