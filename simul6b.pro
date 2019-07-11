#-------------------------------------------------
#
# Project created by QtCreator 2019-07-09T12:03:32
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simul6b
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14


msvc: QMAKE_CXXFLAGS += /openmp


SOURCES += \
    main.cpp \
    simul6.cpp \
    computecontrol.cpp \
    simulationprofile.cpp \
    Constituent.cpp \
    ConstituentDb.cpp \
    Engine.cpp \
    Mix.cpp \
    Sample.cpp \
    Vec2d.cpp \
    pch.cpp \
    json11.cpp \
    graf.cpp \
    inputparameters.cpp \
    constituentsmodel.cpp \
    constituentsdialog.cpp \
    segmentsmodel.cpp

HEADERS += \
    simul6.h \
    computecontrol.h \
    simulationprofile.h \
    Constituent.h \
    ConstituentDb.h \
    Engine.h \
    Mix.h \
    Sample.h \
    Vec2d.h \
    pch.h \
    json11.hpp \
    graf.h \
    inputparameters.h \
    constituentsmodel.h \
    settings.h \
    constituentsdialog.h \
    segmentsmodel.h

FORMS += \
    simul6.ui \
    computecontrol.ui \
    simulationprofile.ui \
    inputparameters.ui \
    constituentsdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
