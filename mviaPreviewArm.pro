#-------------------------------------------------
#
# Project created by QtCreator 2012-04-10T11:37:13
#
#-------------------------------------------------
# qmake -spec /usr/local/angstrom/arm_qt_gles/arm-angstrom-linux-gnueabi/usr/share/qt4/mkspecs/linux-g++ -o Makefile.Arm mviaPreviewArm.pro
# OE_QMAKE_CC   = arm-angstrom-linux-gnueabi-gcc
# OE_QMAKE_CXX  = arm-angstrom-linux-gnueabi-g++
# OE_QMAKE_LINK = arm-angstrom-linux-gnueabi-g++
# OE_QMAKE_INCDIR_QT = /usr/local/angstrom/arm/arm-angstrom-linux-gnueabi/usr/include/qt4
# OE_QMAKE_LIBDIR_QT = /usr/local/angstrom/arm/arm-angstrom-linux-gnueabi/usr/lib
# OE_QMAKE_MOC = moc
# OE_QMAKE_RCC = rcc
# OE_QMAKE_UIC = uic
#-------------------------------------------------

QT       += core gui

CONFIG += qt thread debug

TARGET = mviaPreviewArm
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    gleswidget.cpp \
    workerthread.cpp

HEADERS  += mainwindow.h \
    gleswidget.h \
    workerthread.h

FORMS    += mainwindow.ui

INCLUDEPATH += $(MVIMPACT_ACQUIRE_SOURCE_DIR) \
               $(OE_QMAKE_INCDIR_QT)/QtOpenGL \
               /usr/local/angstrom/arm_kernel_header/include

unix:LIBS += $(MVIMPACT_ACQUIRE_SOURCE_DIR)/lib/armv7a/libosAbstraction.a \
            -L$(MVIMPACT_ACQUIRE_SOURCE_DIR)/lib/armv7a -lmvDeviceManager -lmvPropHandling -lm -ldl -lrt -lQtOpenGL -lGLESv2

OTHER_FILES += \
    yuv2rgb_blx.frag

DEFINES += QT_OPENGL_LIB \
    ARM
#DEFINES += DOAWBINTERNAL

RESOURCES += \
    mviapreview.qrc



















