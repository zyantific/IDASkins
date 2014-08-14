# name of the project
TARGET = IDASkins

# project type
TEMPLATE = lib

# this is to avoid the version number in the name of the plugin
CONFIG += plugin

# the namespace for Qt
QT_NAMESPACE = QT

# add include path
INCLUDEPATH += $(IDASDK)/include

QT += xml xmlpatterns

# build settings

DEFINES += __IDP__ \
           NO_OBSOLETE_FUNCS \
           QPROJECT_LIBRARY

win32 {
	is64 {
		TARGET_EXT = .p64
	}
	else {
		TARGET_EXT = .plw
	}
    DEFINES += __NT__ \
        _CRT_SECURE_NO_WARNINGS \
        __VC__
    SYSNAME = win
    COMPILER_NAME = vc
}
!mac:unix {
    TARGET_EXT = .plx
    DEFINES += __LINUX__ \
               _FORTIFY_SOURCE=0
    SYSNAME = lnx
    COMPILER_NAME = gcc
    # avoid linking GLIBC_2.11 symbols (longjmp_chk)
    CFLAGS += -D_FORTIFY_SOURCE=0
}
mac { # scope name must be 'mac'
    TARGET_EXT = .pmc
    DEFINES += __MAC__
    SYSNAME = mac
    COMPILER_NAME = gcc
    CONFIG += macx
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.3
    QMAKE_INFO_PLIST = Info.plist

    # we compile and link a 32-bit application:
    !x64 {
      CFLAGS = -m32
      QMAKE_LFLAGS_DEBUG += -m32
      QMAKE_LFLAGS_RELEASE += -m32
    }
}

CONFIG(debug, debug|release) {
  DEFINES += _DEBUG
}
else {
  #OPTSUF=_opt
}

x64 {
    TARGET_PROCESSOR_NAME = x64
    DEFINES += __EA64__ __X64__
    SUFF64 = 64x
    ADRSIZE = 64
}
else {
    TARGET_PROCESSOR_NAME = x86
    is64 {
        DEFINES += __EA64__
        SUFF64 = 64
        ADRSIZE = 64
    }
    else {
        ADRSIZE = 32
    }
}

SYSDIR = $${TARGET_PROCESSOR_NAME}_$${SYSNAME}_$${COMPILER_NAME}_$${ADRSIZE}$${OPTSUF}
OBJDIR = obj/$${SYSDIR}/

# set rpath on linux
linux:LIBS += -z \
    defs \
    -z \
    origin \
    -z \
    now \
    -Wl,-rpath=\'\$\$ORIGIN\'

# add library directory
LIBDIR = $$(IDASDK)/lib/$${SYSDIR}/
LIBS += -L"$${LIBDIR}" -lida

# set all build directories
MOC_DIR = $${OBJDIR}
OBJECTS_DIR = $${OBJDIR}
RCC_DIR = $${OBJDIR}
UI_DIR = $${OBJDIR}

# set the destination directory for the binary
DESTDIR = "$(IDADIR)/plugins/"

# run install_name_tool after linking
# mac:QMAKE_POST_LINK += install_name_tool -change /idapathsample/libida$${SUFF64}.dylib @executable_path/libida$${SUFF64}.dylib $(TARGET)
#                        do the same for the Qt libraries

# on unix systems make sure to copy the file with the correct name,
# since qmake adds a prefix and suffix, and ignores TARGET_EXT
unix {
    MY_TARGET = ${DESTDIR}$${TARGET}$${TARGET_EXT}
    COPY_TARGET = $(COPY_FILE) ${DESTDIR}${TARGET} $$MY_TARGET
    QMAKE_POST_LINK += $$COPY_TARGET
}

#--------------------------------------------------------------------------
#
# add project files here
#
#--------------------------------------------------------------------------

HEADERS += ThemeSelector.hpp                \
           Utils.hpp                        \
           ThemeManifest.hpp                \
           Settings.hpp                     \
           Config.hpp                       \

SOURCES += ThemeSelector.cpp                \
           Utils.cpp                        \
           ThemeManifest.cpp                \
           Settings.cpp                     \
           idaskins.cpp                     \

FORMS += ui/ThemeEditorDialog.ui            \
         ui/ThemeSelector.ui                \