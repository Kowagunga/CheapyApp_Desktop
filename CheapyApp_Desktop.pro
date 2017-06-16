TEMPLATE = subdirs

SUBDIRS += \
    CheapyApp_Source

CONFIG(debug, debug|release) {
    SUBDIRS += CheapyApp_Test
    message("Compiled for testing")
}

OTHER_FILES += README.md \
    += LICENSE.txt
