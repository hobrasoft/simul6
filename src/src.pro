#-------------------------------------------------
#
# Project created by QtCreator 2019-07-09T12:03:32
#
#-------------------------------------------------

QT += core gui charts

# CONFIG += debug debug_and_release

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simul6b
TEMPLATE = app
DESTDIR = ../bin

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

msvc { 
    QMAKE_CXXFLAGS += /openmp
    DEFINES += PDEBUG_FUNCTION_NAME=__FUNCSIG__
} else {
    QMAKE_CXXFLAGS += -ftree-parallelize-loops=16 -fopenmp
    DEFINES += PDEBUG_FUNCTION_NAME=__PRETTY_FUNCTION__
    LIBS += -lgomp
}

include(../version.pri)
include(../global.pri)
include(../data/data.pri)
include(../images/images.pri)
include(utils/utils.pri)
include(widgets/widgets.pri)
include(database/database.pri)

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

DEFINES += APP_VERSION=\\\"$$APP_VERSION\\\"
DEFINES += APP_NAME=\\\"$$APP_NAME\\\"
DEFINES += ORGANIZATION_DOMAIN=\\\"$$ORGANIZATION_DOMAIN\\\"

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
    graf.cpp \
    grafdetail.cpp \
    constituentseries.cpp \
    inputparameters.cpp \
    constituentsmodel.cpp \
    constituentsdialog.cpp \
    segmentsmodel.cpp \
    segmentsdelegate.cpp \
    preferences.cpp \
    parametersmodel.cpp \
    mixcontrol.cpp \
    mixcontrolmodel.cpp \
    mixcontroldelegate.cpp \
    simulationprogressdialog.cpp \
    saveprogress.cpp \
    segments.cpp \
    importsna.cpp \
    ampholines.cpp \
    myview.cpp

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
    graf.h \
    grafdetail.h \
    constituentseries.h \
    inputparameters.h \
    constituentsmodel.h \
    settings.h \
    constituentsdialog.h \
    segmentsmodel.h \
    segmentsdelegate.h \
    preferences.h \
    parametersmodel.h \
    mixcontrol.h \
    mixcontrolmodel.h \
    mixcontroldelegate.h \
    simulationprogressdialog.h \
    saveprogress.h \
    segments.h \
    importsna.h \
    ampholines.h \
    myview.h

FORMS += \
    simul6.ui \
    computecontrol.ui \
    simulationprofile.ui \
    inputparameters.ui \
    constituentsdialog.ui \
    preferences.ui \
    mixcontrol.ui \
    grafdetail.ui \
    ampholines.ui \
    simulationprogressdialog.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
