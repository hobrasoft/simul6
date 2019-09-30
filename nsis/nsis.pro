TEMPLATE = aux
TARGET =
CONFIG += no_link target_predeps staticlib

TEMPNAME = $${QMAKE_QMAKE}
QTPATH = $$system_path($$dirname(TEMPNAME))
GCCPATH = $$system_path($$clean_path($${QTPATH}\..\..\..\Tools\mingw530_32\bin))
BINPATH = $$system_path($$clean_path($${OUT_PWD}\..\bin\\))
MAKENSIS = C:\Program Files (x86)\NSIS\Bin\makensis.exe

QMAKE_EXTRA_TARGETS += first

include(../version.pri)
first.commands += \
    powershell -Command \
        "\"Copy-Item\"" -Path $${PWD}\\simul6b.nsi -Destination $${OUT_PWD} & \
    powershell -Command \
        \"(gc $$PWD/NSIS.definitions.nsh.in) \
        -replace \\\"\"@APP_NAME@\"\\\", \\\"$$APP_NAME\\\" \
        -replace \\\"\"@APP_VERSION@\"\\\", \\\"$$APP_VERSION\\\" \
        -replace \\\"\"@EXE_NAME@\"\\\", \\\"$$EXE_NAME\\\" \
        -replace \\\"\"@PWD@\"\\\", \\\"$$PWD\\\" \
        -replace \\\"\"@OUT_PWD@\"\\\", \\\"$$OUT_PWD\\\" \
        -replace \\\"\"@APP_LOW_NAME@\"\\\", \\\"$$APP_LOW_NAME\\\" | \
            Out-File $$OUT_PWD\NSIS.definitions.nsh\" & \
    powershell -Command \
        "\"Copy-Item\"" -Path $${GCCPATH}\\libgomp-1.dll -Destination $${BINPATH} & \
    powershell -Command \
        "\"Copy-Item\"" -Path $${GCCPATH}\\libgcc_s_dw2-1.dll -Destination $${BINPATH} & \
    powershell -Command \
        "\"Copy-Item\"" -Path $${GCCPATH}\\libstdc++-6.dll -Destination $${BINPATH} & \
    \"$$QTPATH\windeployqt.exe\" $${BINPATH} & \
    \"$$MAKENSIS\" simul6b.nsi

DISTFILES += \
    NSIS.definitions.nsh.in \
    simul6b.nsi


