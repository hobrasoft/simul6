INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD

QT += sql

SOURCES += \
 $$PWD/database.cpp \
 $$PWD/msqlquery.cpp \


HEADERS += \
 $$PWD/db.h \
 $$PWD/dbt.h \
 $$PWD/database.h \
 $$PWD/msqlquery.h \

include(sqlite/sqlite.pri)

