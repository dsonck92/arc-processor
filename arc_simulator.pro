TEMPLATE = app
TARGET = arc_simulator
QT += core \
    gui \
    widgets
HEADERS += microfileeditor.h \
    ARCMicroStore.h \
    ARCScratchpad.h \
    ARCMemory.h \
    memoryeditor.h \
    ARCProcessor.h \
    registereditor.h \
    processorcontrol.h \
    programeditor.h \
    arc_simulator.h
SOURCES += microfileeditor.cpp \
    ARCMicroStore.cpp \
    ARCScratchpad.cpp \
    ARCMemory.cpp \
    memoryeditor.cpp \
    ARCProcessor.cpp \
    registereditor.cpp \
    processorcontrol.cpp \
    programeditor.cpp \
    main.cpp \
    arc_simulator.cpp
FORMS += microfileeditor.ui \
    memoryeditor.ui \
    registereditor.ui \
    processorcontrol.ui \
    programeditor.ui \
    arc_simulator.ui
RESOURCES += resource.qrc
