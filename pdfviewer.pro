QT       += core gui
QT += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Reader11
TEMPLATE = app

FORMS         = window.ui \
    noteitemwidget.ui \
    qdataprovider.ui \
    qsearchthread.ui
HEADERS       = documentwidget.h \
                window.h \
    pdfkit.h \
    scrollareacontentwidget.h \
    noteitemwidget.h \
    qdataprovider.h \
    qdatathread.h \
    qsearchthread.h
SOURCES       = documentwidget.cpp \
                main.cpp \
                window.cpp \
    pdfkit.cpp \
    scrollareacontentwidget.cpp \
    noteitemwidget.cpp \
    qdataprovider.cpp \
    qdatathread.cpp \
    qsearchthread.cpp

RESOURCES += \
    pdfviewer.qrc

INCLUDEPATH += $$PWD/_Inclusion/Win32/poppler
win32: LIBS += -L$$PWD/_Inclusion/Win32/poppler -llibpoppler
win32: LIBS += -L$$PWD/_Inclusion/Win32/poppler -llibpoppler-qt5

unix: PKGCONFIG += poppler-qt5
unix: CONFIG += c++11 link_pkgconfig

DISTFILES +=
