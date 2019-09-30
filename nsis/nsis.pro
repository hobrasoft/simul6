TEMPLATE = aux
TARGET =
CONFIG += no_link target_predeps staticlib

QMAKE_EXTRA_TARGETS += first

include(../version.pri)
first.commands += \
    powershell \
    -Command \"(gc $$PWD/NSIS.definitions.nsh.in) \
        -replace \\\"\"@APP_NAME@\"\\\", \\\"$$APP_NAME\\\" \
        -replace \\\"\"@APP_VERSION@\"\\\", \\\"$$APP_VERSION\\\" \
        -replace \\\"\"@EXE_NAME@\"\\\", \\\"$$EXE_NAME\\\" \
        -replace \\\"\"@PWD@\"\\\", \\\"$$PWD\\\" \
        -replace \\\"\"@OUT_PWD@\"\\\", \\\"$$OUT_PWD\\\" \
        -replace \\\"\"@APP_LOW_NAME@\"\\\", \\\"$$APP_LOW_NAME\\\" | \
            Out-File $$OUT_PWD\NSIS.definitions.nsh\" & \
        \"C:\Program Files (x86)\NSIS\Bin\makensis.exe\" simul6b.nsi

