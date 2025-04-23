QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 precompile_header

# Use Precompiled headers (PCH)
PRECOMPILED_HEADER  = _msvcStuff/stdafx.h

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += \
	A0S_QT_PRO=1

INCLUDEPATH += \
	src

SOURCES += \
    HelloQt.cpp

HEADERS += \
	src/HelloQt.h \
    src/Common.h \
    src/HelloQt.h \
    src/QrcLocatorReplacement.h \
    src/RegistratorLoader.h \
    src/Scener.h \
    src/Simulation/AllRoads.h \
    src/Simulation/Curve.h \
    src/Simulation/Draw.h \
    src/Simulation/IVehicle.h \
    src/Simulation/Road.h \
    src/Simulation/Vehicle.h \
    src/Simulation/VehicleGenerator.h \
    src/Timing.h \
    src/Updater.h \
    src/viaQGraphicsView.h \
    src/viaQQuickItem.h \
    src/viaQQuickPaintedItem.h \
    src/Via/GraphicsView/BaseQGraphicsView.h \
    src/Via/GraphicsView/DraggableQGraphicsView.h \
    src/Via/GraphicsView/LoopLauncherQGraphicsView.h \
    src/Via/GraphicsView/MainQGraphicsView.h \
    src/Via/GraphicsView/MainWindow.h \
    src/Via/GraphicsView/uic/ui_mainwindow.h \
    src/Via/GraphicsView/ZoomableQGraphicsView.h \
    src/Via/QuickItem/BaseQQuickItem.h \
    src/Via/QuickItem/DraggableQQuickItem.h \
    src/Via/QuickItem/MainQQuickItem.h \
    src/Via/QuickItem/ZoomableQQuickItem.h \
    src/Via/QuickPaintedItem/BaseQQuickPaintedItem.h \
    src/Via/QuickPaintedItem/DraggableQQuickPaintedItem.h \
    src/Via/QuickPaintedItem/MainQQuickPaintedItem.h \
    src/Via/QuickPaintedItem/ZoomableQQuickPaintedItem.h \
    _msvcStuff/stdafx.h

FORMS += \
    resource/mainwindow.ui

# Output
UI_DIR = src/Via/GraphicsView/uic

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
