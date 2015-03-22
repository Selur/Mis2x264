TEMPLATE = app
TARGET = mis2x264
QT += core \
    gui
win32 {
  DEFINES += BUILDVERSION=\\\"$$system('echo %date:~6,4%.%date:~3,2%.%date:~0,2%').1\\\"
  
 } else: {
  DEFINES += BUILDVERSION=\\\"$$system(date '+%y%m.%d.1')\\\"
  macx:DEFINES += MACBUILDDATE=\\\"$$system(date '+%y%m.%d.1')\\\"
} 

#Qt 5+ adjustments
greaterThan(QT_MAJOR_VERSION, 4) { # QT5+
    QT += widgets # for all widgets
    QT += multimedia # for QSound
    win32-msvc*:DEFINES += NOMINMAX
}

# OS specific adjustments
win32-msvc* { 
    message(Building for Windows using Qt $$QT_VERSION)
    CONFIG += c++11 # C++11 support
    QMAKE_CXXFLAGS += /bigobj # allow big objects
    !contains(QMAKE_HOST.arch, x86_64):QMAKE_LFLAGS += /LARGEADDRESSAWARE # allow the use more of than 2GB of RAM on 32bit Windows
    
    # # add during static build
    # QMAKE_CFLAGS_RELEASE += -MT
    # QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO += -MT
    # QMAKE_CFLAGS_DEBUG = -Zi -MTd
    # QMAKE_LFLAGS += /DYNAMICBASE:NO
    
    # for Windows XP compatibility
    contains(QMAKE_HOST.arch, x86_64):QMAKE_LFLAGS += /SUBSYSTEM:WINDOWS,5.02 # Windows XP 64bit
    else:QMAKE_LFLAGS += /SUBSYSTEM:WINDOWS,5.01 # Windows XP 32bit
}
else:linux-* { 
    message(Building for Linux using Qt $$QT_VERSION)
    QT += dbus # dbus support
    QMAKE_CXXFLAGS += -std=c++0x
}
else:macx { 
    message(Building for Mac OS X using Qt $$QT_VERSION)
    LIBS += -framework \
        Cocoa
    QMAKE_CFLAGS_RELEASE += -fvisibility=hidden
    QMAKE_CXXFLAGS += -std=c++0x
    QMAKE_CXXFLAGS_RELEASE += -fvisibility=hidden \
        -fvisibility-inlines-hidden
    HEADERS += Frontend/MacDockTile.h \
        Frontend/CocoaInitializer.h \
        Helper/MacHelper.h
    OBJECTIVE_SOURCES += Frontend/MacDockTile.mm \
        Frontend/CocoaInitializer.mm \
        Helper/MacHelper.mm
    QMAKE_INFO_PLIST = resources/Info.plist
}
else:message(Building for UNKNOWN using Qt $$QT_VERSION)

HEADERS += Converter.h \
    Mis2x264.h
SOURCES += Converter.cpp \
    main.cpp \
    Mis2x264.cpp
FORMS += Mis2x264.ui
RESOURCES += 
