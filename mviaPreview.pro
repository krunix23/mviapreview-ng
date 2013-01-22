#-------------------------------------------------
#
# Project created by QtCreator 2012-04-10T11:37:13
#
#-------------------------------------------------

QT       += core gui opengl

CONFIG += qt thread debug

TARGET = mviaPreview
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    gleswidget.cpp \
    workerthread.cpp

HEADERS  += mainwindow.h \
    gleswidget.h \
    workerthread.h

FORMS    += mainwindow.ui

INCLUDEPATH += $(MVIMPACT_ACQUIRE_SOURCE_DIR)

unix:LIBS += $(MVIMPACT_ACQUIRE_SOURCE_DIR)/lib/x86/libosAbstraction.a \
             -L$(MVIMPACT_ACQUIRE_SOURCE_DIR)/lib/x86 -lmvDeviceManager -lmvPropHandling -lm -ldl -lrt

win32:LIBS += $(MVIMPACT_ACQUIRE_SOURCE_DIR)/lib/mvDeviceManager.lib \
              $(MVIMPACT_ACQUIRE_SOURCE_DIR)/lib/win/win32/mvPropHandling.lib

OTHER_FILES += \
    yuv2rgb_mvia.frag
    
RESOURCES += \
    mviapreview.qrc










