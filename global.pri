CONFIG += debug

system( $$QMAKE_CXX --version | grep -e "\\ 4\\.[0-8]") {
    message( "g++ version 4.8 or earlier found" )
    QMAKE_CXXFLAGS += -fstack-protector-all
    }
system( $$QMAKE_CXX --version | grep -e "\\ 4\\.9") {
    message( "g++ version 4.9 found" )
    QMAKE_CXXFLAGS += -fstack-protector-strong
    }
system( $$QMAKE_CXX --version | grep -e "\\ 5\\.[0-9]") {
    message( "g++ version 5.0 or later found" )
    QMAKE_CXXFLAGS += -fstack-protector-strong -fPIE -pie
    }
system( $$QMAKE_CXX --version | grep -e "\\ 6\\.[0-9]") {
    message( "g++ version 6.0 or later found" )
    QMAKE_CXXFLAGS += -fstack-protector-strong -fPIE -pie -fsanitize=bounds-strict -fno-delete-null-pointer-checks -flto=9
    }


QMAKE_CXXFLAGS += -std=c++14

OBJECTS_DIR = .tmp
MOC_DIR     = .tmp
RCC_DIR     = .tmp
UI_DIR      = .tmp


