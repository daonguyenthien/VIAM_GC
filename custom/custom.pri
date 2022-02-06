QT += core \
      quick
CONFIG += c++11
message("Adding Custom Plugin")

# Major and minor versions are defined here
CUSTOM_QGC_VERSION = 0.1.0
message(VIAM-AUV-GC $${CUSTOM_QGC_VERSION})
DEFINES -= GIT_VERSION=\"\\\"$$GIT_VERSION\\\"\"
DEFINES += GIT_VERSION=\"\\\"$$CUSTOM_QGC_VERSION\\\"\"

# Disable APM and PX4 support
CONFIG  += QGC_DISABLE_PX4_PLUGIN QGC_DISABLE_PX4_PLUGIN_FACTORY
CONFIG  += QGC_DISABLE_APM_PLUGIN QGC_DISABLE_APM_PLUGIN_FACTORY

# Disable support for other stuffs
CONFIG  += QGC_DISABLE_BLUETOOTH
CONFIG  += QGC_DISABLE_NFC QGC_DISABLE_QTNFC QGC_DISABLE_QTNFC

# Configure custom mavlink dialect
DEFINES  += NO_ARDUPILOT_DIALECT
MAVLINKPATH_REL = custom/mavlink
MAVLINKPATH     = $$BASEDIR/$$MAVLINKPATH_REL
MAVLINK_CONF    = viamlab_auv

# Branding
TARGET   = CustomQGC
DEFINES += CUSTOMHEADER=\"\\\"CustomPlugin.h\\\"\"
DEFINES += CUSTOMCLASS=CustomPlugin
DEFINES += QGC_APPLICATION_NAME=\"\\\"VIAM-AUV-GC\\\"\"
DEFINES += QGC_ORG_NAME=\"\\\"viamlab.com\\\"\"
DEFINES += QGC_ORG_DOMAIN=\"\\\"viamlab.com\\\"\"

QGC_APP_NAME        = "VIAM-AUV-GC"
QGC_BINARY_NAME     = "ViamAUV_QGC"
QGC_ORG_NAME        = "VIAM Lab"
QGC_ORG_DOMAIN      = "viamlab.com"
QGC_APP_DESCRIPTION = "Ground Station Control for AUV"
QGC_APP_COPYRIGHT   = "Copyright (C) 2020 VIAM Laboratory Development Team. All rights reserved."

# Our own, custom resources
RESOURCES += \
    $$PWD/custom.qrc \
    $$PWD/custom.qrc 

QML_IMPORT_PATH += \
   $$PWD/qml
   $$PWD/res

# Include headers and sources
INCLUDEPATH += \
    $$PWD/src 

HEADERS += \
    $$PWD/src/CustomPlugin.h \
    $$PWD/src/Custom/PathController.h \
    $$PWD/src/Custom/PathPlanningDubins.h \
    $$PWD/src/Custom/PathPlanningBsplines.h \
    $$PWD/src/Custom/TSP/TSP_Basic.h \
    $$PWD/src/Custom/TSP/TSP_GA.h \
    $$PWD/src/Custom/TSP/TSP_NN.h \
    $$PWD/src/Custom/PathPlanning.h \

SOURCES += \
    $$PWD/src/CustomPlugin.cc \
    $$PWD/src/Custom/PathController.cc \
    $$PWD/src/Custom/PathPlanningDubins.cc \
    $$PWD/src/Custom/PathPlanningBsplines.cc \
    $$PWD/src/Custom/TSP/TSP_Basic.cpp \
    $$PWD/src/Custom/TSP/TSP_GA.cpp \
    $$PWD/src/Custom/TSP/TSP_NN.cpp \
    $$PWD/src/Custom/PathPlanning.cc \
    
# Custom Firmware/AutoPilot Plugin
INCLUDEPATH += \
    $$QGCROOT/custom/src/FirmwarePlugin \
    $$QGCROOT/custom/src/AutoPilotPlugin

HEADERS+= \
    $$QGCROOT/custom/src/AutoPilotPlugin/CustomAutoPilotPlugin.h \
    $$QGCROOT/custom/src/FirmwarePlugin/CustomFirmwarePlugin.h \
    $$QGCROOT/custom/src/FirmwarePlugin/CustomFirmwarePluginFactory.h

SOURCES += \
    $$QGCROOT/custom/src/AutoPilotPlugin/CustomAutoPilotPlugin.cc \
    $$QGCROOT/custom/src/FirmwarePlugin/CustomFirmwarePlugin.cc \
    $$QGCROOT/custom/src/FirmwarePlugin/CustomFirmwarePluginFactory.cc

DISTFILES += \
    $$PWD/res/CustomFlyViewOverlay.qml\
    $$PWD/qml/CustomSliderMassPiston.qml \
    $$PWD/qml/BatteryHeadIndicator.qml \
    $$PWD/qml/BatteryTailIndicator.qml \
    $$PWD/qml/SpeedNeedle.qml \
    $$PWD/qml/SpeedMeter.qml \
    $$PWD/qml/CustomThrusterFlightView.qml \
    $$PWD/qml/AngleNeedle.qml \
    $$PWD/qml/RudderMeter.qml \
    $$PWD/qml/CustomRudderStatus.qml \
    $$PWD/res/collapse.png \
    $$PWD/res/Custom_clock.png \
    $$PWD/res/Custom_rudder.png \
    $$PWD/res/expand.png \
    $$PWD/res/APMBrandImageAUV2000.png \
    $$PWD/res/viam_auv2000_full_white.svg
