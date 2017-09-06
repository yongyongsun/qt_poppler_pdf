/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the documentation of Qt. It was originally
** published as part of Qt Quarterly.
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include "documentwidget.h"
#include "ui_window.h"
#include "pagelabel.h"
#include "pdfkit.h"

class QTextBrowser;
class ScrollAreaContentWidget;
class QDataThread;
class Window : public QMainWindow, public Ui_MainWindow
{
    Q_OBJECT

public:
    Window(QWidget *parent = 0);
    void setScale(qreal scale);
    QMatrix matrix() const;

protected:
    void resizeEvent(QResizeEvent* e);

public slots:
    void openFileAuto(const QString &path);

private slots:
    void checkSearchText(const QString &text);
    void openFile(const QString& path = "");
    void scaleDocument(int index);
    void searchDocument();
    void onShowSelectedText(const QString &text);
    void onValuechange(int value);
    void onBtnNoteClicked();
    void closeEvent(QCloseEvent *event);
    void onSearchDataShow(const QVector<SearchData>& vec);
    void onListWidgetClicked(QListWidgetItem * item);

private:
    DocumentWidget *documentWidget;
    QString lastPath;
    QVector<qreal> scaleFactors;
    int  m_curPage;
    PageLabel *mplable;
    QScrollArea *scrollArea;
    ScrollAreaContentWidget *scrollAreaWidgetContents;
    QDataThread *m_pThread;
    QString m_strSearchText;
};

#endif
