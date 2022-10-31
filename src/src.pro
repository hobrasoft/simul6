#-------------------------------------------------
#
# Project created by QtCreator 2019-07-09T12:03:32
#
#-------------------------------------------------

QT += core gui charts

CONFIG += debug debug_and_release

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simul6
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
} 
macx {
    QMAKE_CXXFLAGS += -Xpreprocessor -fopenmp
    DEFINES += PDEBUG_FUNCTION_NAME=__PRETTY_FUNCTION__
    # Look for libomp installed with Homebrew
    INCLUDEPATH += /usr/local/opt/libomp/include
    LIBS += -L/usr/local/opt/libomp/lib
    # Look for libomp installed with MacPorts
    INCLUDEPATH += /opt/local/include/libomp
    LIBS += -L/opt/local/lib/libomp
    LIBS += -lomp
    TARGET = "Simul 6"
    ICON = ../images/appicon.icns
} 
unix:!macx {
    QMAKE_CXXFLAGS += -ftree-parallelize-loops=24 -fopenmp
    DEFINES += PDEBUG_FUNCTION_NAME=__PRETTY_FUNCTION__
    LIBS += -lgomp
}
#cwin32 {
#   QMAKE_CXXFLAGS += -ftree-parallelize-loops=24 -fopenmp
#   DEFINES += PDEBUG_FUNCTION_NAME=__PRETTY_FUNCTION__
#   LIBS += -lgomp
#}

include(../version.pri)
include(../global.pri)
include(../data/data.pri)
include(../images/images.pri)
include(database/database.pri)
include(utils/utils.pri)
include(widgets/widgets.pri)

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
    grafabstract.cpp \
    grafdetail.cpp \
    detector.cpp \
    detectorcache.cpp \
    vacourse.cpp \
    vacoursecache.cpp \
    constituentseries.cpp \
    inputparameters.cpp \
    constituentsmodel.cpp \
    constituentsdialog.cpp \
    segmentsmodel.cpp \
    segmentsdelegate.cpp \
    preferences.cpp \
    mixcontrol.cpp \
    mixcontroltab.cpp \
    mixcontrolmodel.cpp \
    mixcontrolswapmodel.cpp \
    mixcontrolmodelabstract.cpp \
    mixcontroldelegate.cpp \
    saveprogress.cpp \
    segmentedconstituent.cpp \
    importsna.cpp \
    ampholines.cpp \
    replay.cpp \
    replaydatajson.cpp \
    replaydatasqlite3.cpp \
    myview.cpp \
    manualscale.cpp \
    swapsegmentwidget.cpp \
    swapsegmentwidgetchart.cpp \
    crosssectiondialog.cpp \
    crosssectionmodel.cpp \
    linesmanager.cpp \
    linesmodel.cpp \
    linesdelegate.cpp \

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
    grafabstract.h \
    grafdetail.h \
    detector.h \
    detectorcache.h \
    vacourse.h \
    vacoursecache.h \
    constituentseries.h \
    conductivityseries.h \
    phseries.h \
    electricfieldseries.h \
    currentseries.h \
    voltageseries.h \
    inputparameters.h \
    constituentsmodel.h \
    settings.h \
    constituentsdialog.h \
    segmentsmodel.h \
    segmentsdelegate.h \
    preferences.h \
    mixcontrol.h \
    mixcontroltab.h \
    mixcontrolmodel.h \
    mixcontrolswapmodel.h \
    mixcontrolmodelabstract.h \
    mixcontroldelegate.h \
    saveprogress.h \
    segmentedconstituent.h \
    importsna.h \
    ampholines.h \
    replay.h \
    replaydataabstract.h \
    replaydatajson.h \
    replaydatasqlite3.h \
    grafstyle.h \
    myview.h \
    manualscale.h \
    swapsegmentwidget.h \
    swapsegmentwidgetchart.h \
    crosssectiondialog.h \
    crosssectionmodel.h \
    crosssection.h \
    drawingflag.h \
    linesmanager.h \
    linesmodel.h \
    linesdelegate.h \

FORMS += \
    simul6.ui \
    computecontrol.ui \
    simulationprofile.ui \
    inputparameters.ui \
    constituentsdialog.ui \
    preferences.ui \
    mixcontrol.ui \
    mixcontroltab.ui \
    grafdetail.ui \
    ampholines.ui \
    replay.ui \
    saveprogress.ui \
    manualscale.ui \
    swapsegmentwidget.ui \
    crosssectiondialog.ui \
    linesmanager.ui \

RESOURCES += \
    simul6.qrc \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
