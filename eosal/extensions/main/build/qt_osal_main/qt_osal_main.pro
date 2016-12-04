#***************************************************************************************************
#
# OSAL main project file for qmake (Nokia QT make)
#
#***************************************************************************************************

!include( ../../../../build/qt_defs/qt_osal_library_defs.txt ) {
   message( "No qt_osal_library_defs.txt file found" )
}

TARGET = osal_main

win32 {
  SOURCES += ../../win32/osal_main.c
}

HEADERS += ../../osal_main.h
