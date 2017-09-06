#include <QtGui>
#include "window.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include "ScrollAreaContentWidget.h"
#include "qdatathread.h"

Window::Window(QWidget *parent)
    : QMainWindow(parent)
    , scrollArea(NULL)
    , scrollAreaWidgetContents(NULL)
    , m_pThread(NULL)
    , m_strSearchText("")
{
    setupUi(this);
    m_pThread = new QDataThread(this);
    qRegisterMetaType<vecSearchData>("vecSearchData");
    scaleFactors << 0.25 << 0.5 << 0.75 << 1. << 1.25 << 1.5 << 2. << 3. << 4.;

    addDockWidget(Qt::LeftDockWidgetArea, selectionDockWidget);

    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

    connect(documentControlsAction, SIGNAL(toggled(bool)),controlsDockWidget, SLOT(setVisible(bool)));
    connect(selectedTextAction, SIGNAL(toggled(bool)),selectionDockWidget, SLOT(setVisible(bool)));
    connect(controlsDockWidget, SIGNAL(visibilityChanged(bool)),documentControlsAction, SLOT(setChecked(bool)));
    connect(selectionDockWidget, SIGNAL(visibilityChanged(bool)),selectedTextAction, SLOT(setChecked(bool)));

    connect(listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(onListWidgetClicked(QListWidgetItem *)));

    connect(scaleComboBox, SIGNAL(currentIndexChanged(int)),this, SLOT(scaleDocument(int)));


    connect(searchLineEdit, SIGNAL(returnPressed()), this, SLOT(searchDocument()));
    connect(findButton, SIGNAL(clicked()), this, SLOT(searchDocument()));
    connect(searchLineEdit, SIGNAL(textChanged(const QString &)),this, SLOT(checkSearchText(const QString &)));

    connect(PdfKit::GetInstance(),SIGNAL(textSelected(const QString &)),
            this,SLOT(onShowSelectedText(const QString &)),Qt::QueuedConnection);

    connect(PdfKit::GetInstance(),SIGNAL(eSearchData(const QVector<SearchData>&)),
            this,SLOT(onSearchDataShow(const QVector<SearchData>&)),Qt::QueuedConnection);

    connect(btnNote, SIGNAL(clicked()), this, SLOT(onBtnNoteClicked()));

    selectedTextAction->setChecked(false);

    scrollArea = new QScrollArea(centralwidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignCenter);
    verticalLayout->addWidget(scrollArea);
    scrollAreaWidgetContents = new ScrollAreaContentWidget(scrollArea);
    scrollArea->setWidget(scrollAreaWidgetContents);
    connect(scrollArea->verticalScrollBar(),SIGNAL(sliderMoved(int)),this,SLOT(onValuechange(int)));

    //初始化线程数据加载
    qDebug()<<"loadData current thread id:"<<(int)QThread::currentThreadId();
    m_pThread->start();
}

void Window::onBtnNoteClicked()
{
     scrollAreaWidgetContents->prepareAddNoteItem();
}

void Window::onValuechange(int value)
{
//    mplable->setCurLabelPage(10);
//    mplable->ShowCurSacleImg();
//    mplable->move(10,value);
}

void Window::checkSearchText(const QString &text)
{
//    if (text.isEmpty())
//    {
//        PdfKit::GetInstance()->search(0,text);
//    }
}


QMatrix Window::matrix() const
{
    return QMatrix(1 * physicalDpiX() / 72.0, 0,
                   0, 1 * physicalDpiY() / 72.0,
                   0, 0);
}

void Window::openFileAuto(const QString &path)
{
    openFile(path);
}

void Window::openFile(const QString& path)
{
    QString strPath;
    if (path == NULL)
    {
        strPath = QFileDialog::getOpenFileName(this, tr("Open PDF File"),
                                                    lastPath, tr("PDF files (*.pdf)"));
    }else{
        strPath = path;
    }

    if (strPath.isEmpty())
        return;

    if (PdfKit::GetInstance()->initLoad(strPath)) {
        lastPath = path;
        searchLineEdit->setEnabled(true);
        searchComboBox->setEnabled(true);
        findButton->setEnabled(true);
        clearButton->setEnabled(true);
        scaleComboBox->setEnabled(true);
        pageSpinBox->setEnabled(true);
        pageSpinBox->setMinimum(1);
        pageSpinBox->setMaximum(PdfKit::GetInstance()->getPageNum());
        pageSpinBox->setValue(1);
    } else
        QMessageBox::warning(this, tr("PDF Viewer - Failed to Open File"),
                             tr("The specified file could not be opened."));



    int nPageTotal = PdfKit::GetInstance()->getPageNum();
    QSize widgetSize = PdfKit::GetInstance()->getSumPageSize(nPageTotal);
    qDebug()<<"current windows size = "<<widgetSize.width()<<"  "<<widgetSize.height();
    scrollAreaWidgetContents->setFixedSize(widgetSize);

}

void Window::scaleDocument(int index)
{
    qreal value = scaleFactors.at(index);
    PdfKit::GetInstance()->Setscale(value);

    //调整滚动区域大小
    qreal nmax = scrollArea->verticalScrollBar()->maximum();
    int nCurPos = scrollArea->verticalScrollBar()->value();
    int nPageTotal = PdfKit::GetInstance()->getPageNum();
    QSize widgetSize = PdfKit::GetInstance()->getSumPageSize(nPageTotal);
    scrollAreaWidgetContents->setFixedSize(widgetSize);

    qreal nAfterMax = scrollArea->verticalScrollBar()->maximum();
    qreal v = nAfterMax/nmax;
    scrollArea->verticalScrollBar()->setValue(nCurPos * v);

    scrollAreaWidgetContents->update();
}

void Window::searchDocument()
{
    QString text = searchLineEdit->text();
    if (text.isEmpty() || m_strSearchText == text)
        return;

    m_strSearchText = text;
    PdfKit::GetInstance()->SetSearchText(text);
    scrollAreaWidgetContents->update();
}
void Window::onShowSelectedText(const QString &text)
{
    //selectedTextAction->setChecked(true);
}

void Window::onSearchDataShow(const QVector<SearchData>& vec)
{
    listWidget->clear();

    for (int i = 0; i < vec.size(); i++)
    {
        QListWidgetItem *newItem = new QListWidgetItem(vec.at(i).text, listWidget);
        newItem->setData(Qt::UserRole,vec.at(i).pos);
    }
}

void Window::resizeEvent(QResizeEvent* e)
{

}

void Window::closeEvent(QCloseEvent *event)
{
int aa = 2;
}

void Window::onListWidgetClicked(QListWidgetItem * item)
{
    int value  = item->data(Qt::UserRole).toInt();
    scrollArea->verticalScrollBar()->setValue(value);
}
