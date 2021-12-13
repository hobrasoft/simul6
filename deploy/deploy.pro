TEMPLATE = aux
TARGET = $${APP_LOW_NAME}-$${APP_VERSION}-win32.exe

TEMPNAME = $${QMAKE_QMAKE}
QTPATH = $$system_path($$dirname(TEMPNAME))
GCCPATH = $$system_path($$clean_path($${QTPATH}\..\..\..\Tools\mingw530_32\bin))
MSVCPATH = "C:\Program Files (x86)\Microsoft Visual Studio"
BINPATH = $$system_path($$clean_path($${OUT_PWD}\..\bin\\))
MAKENSIS = "D:\NSIS\Bin\makensis.exe"

QMAKE_EXTRA_TARGETS += first

include(../version.pri)
first.commands += \
    \"$$QTPATH\windeployqt.exe\" $${BINPATH} & \
    \"$$MAKENSIS\" simul6b.nsi



