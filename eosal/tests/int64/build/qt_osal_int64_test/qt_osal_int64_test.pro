#***************************************************************************************************
#
# OSAL main project file for qmake (Nokia QT make)
#
#***************************************************************************************************

!include( ../../../../build/qt_defs/qt_osal_example_defs.txt ) {
   message( "No qt_osal_example_defs.txt file found" )
}

TARGET = osal_int64_test

LIBS += -losal_main -losal_utf16 -losal_base

# POST_TARGETDEPS += c:/open-en/lib/win32mingw/libosal_based.a


SOURCES += ../../osal_int64_test.c

