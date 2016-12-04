#***************************************************************************************************
#
# OSAL project file for qmake (Nokia QT make)
#
#***************************************************************************************************

#DEPENDPATH = c:/open-en/
#VPATH = c:/open-en/


# CONFIG += ordered

SUBDIRS = ../../base/build/qt_osal_base \
          ../../modules/build/qt_osal_modules \
          ../../examples/build/qt_osal_examples \
          ../../tests/build/qt_osal_tests

TEMPLATE = subdirs
