#include "pagelabel.h"
#include "pdfkit.h"
#include "window.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QPixmapCache>

PageLabel::PageLabel(QWidget *pWindow,QWidget *parent) :
    QLabel(parent)
    , m_parent((Window*)pWindow)
    , rubberBand(NULL)
    , dragPosition(QPoint())
    , m_curPage(-1)
{

    setAlignment(Qt::AlignCenter);
    m_oldpixMap = QPixmap();
}

PageLabel::~PageLabel()
{

}

void PageLabel::ShowCurSacleImg()
{
    if (m_curPage != -1)
    {
        Poppler::Page* Ppagedata = PdfKit::GetInstance()->getPage(m_curPage);
        int nScale = 1;
        int aaa =  m_parent->physicalDpiY();
        QImage image = Ppagedata->renderToImage(nScale * m_parent->physicalDpiX(), nScale * m_parent->physicalDpiY());
        m_oldpixMap = QPixmap::fromImage(image);
        //this->setGeometry(m_oldpixMap.rect());
        mimage = image;




//        QPixmap pix;
//        QString str = QString("./aaa/aaa_%1.png").arg(m_curPage);
//        pix.load(str);
        this->setPixmap(m_oldpixMap);
        qDebug()<<"QPixmapCache::cacheLimit ()  = " <<QPixmapCache::cacheLimit () ;
//        QString str = QString("./aaa/aaa_%1.png").arg(m_curPage);
//        m_oldpixMap.save(str);
    }
}

void PageLabel::setCurLabelPage(int nvalue)
{
    m_curPage = nvalue;
}

void PageLabel::setSearchListRect(const QList<QRectF> & lists)
{
    m_listSearchRect = lists;
    update();
    //QMetaObject::invokeMethod(this,"onUpdateSearch",Qt::QueuedConnection);
}

void PageLabel::setSelectedRect(QRectF rect)
{
    m_SelectedRect = rect;
    QMetaObject::invokeMethod(this,"onUpdateSelected",Qt::QueuedConnection);
}

void PageLabel::onUpdateSearch()
{
    const QPixmap* CurPixMap = this->pixmap();
    if (CurPixMap->isNull())
        return;

    //图片合成//2>搜索文本合成
    int size = m_listSearchRect.size();
    qDebug()<<"正在搜索第 "<<m_curPage<<" 页"<<"rect size = "<<size;
    if (size == 0)
        return;

    QPixmap temp = CurPixMap->copy();
    if (temp.isNull())
        qDebug()<<"----------------error";
    QPainter painter;
    painter.begin(this);
    for (int i = 0; i<size;i++)
    {
        QRect highlightRect = m_parent->matrix().mapRect(m_listSearchRect.at(i)).toRect();
        highlightRect.adjust(-2, -2, 2, 2);
        painter.fillRect(highlightRect, QColor(0, 50, 0, 32));
    }

    painter.end();
    //setPixmap(temp);
}

void PageLabel::onUpdateSelected()
{
    const QPixmap* CurPixMap = this->pixmap();
    if (CurPixMap->isNull())
        return;

    //图片合成
    //1> selected文本合成
    QRectF Rects = m_parent->matrix().inverted().mapRect(m_SelectedRect);
    QList<QRectF> list = PdfKit::GetInstance()->selectedText(m_curPage,Rects);
    int size = list.size();
    if (0 == size)
        return;

    QPixmap temp = CurPixMap->copy();
    QPainter painter;
    painter.begin(&temp);
    for (int i = 0; i< size;i++)
    {
        QRect highlightRect = m_parent->matrix().mapRect(list.at(i)).toRect();
        highlightRect.adjust(-2, -2, 2, 2);
        painter.fillRect(highlightRect, QColor(50, 0, 0, 32));
    }

    painter.end();
    setPixmap(temp);
}

void PageLabel::onClear()
{
    if (!m_oldpixMap.isNull())
    {
        setPixmap(m_oldpixMap);
    }
}

void PageLabel::mousePressEvent(QMouseEvent *event)
{
    QLabel::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        if (m_SelectedRect.isValid())
        {
            setPixmap(m_oldpixMap);
            m_SelectedRect = QRectF();
        }

        dragPosition = event->pos();
        if (!rubberBand)
            rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        rubberBand->setGeometry(QRect(dragPosition, QSize()));
        rubberBand->show();
    }
}

void PageLabel::mouseMoveEvent(QMouseEvent *event)
{
    QLabel::mouseMoveEvent(event);
    rubberBand->setGeometry(QRect(dragPosition, event->pos()).normalized());
}

void PageLabel::mouseReleaseEvent(QMouseEvent *event)
{
    QLabel::mouseReleaseEvent(event);
    if (!rubberBand->size().isEmpty()) {
        // Correct for the margin around the image in the label.
        QRectF rect = QRectF(rubberBand->pos(), rubberBand->size());
        rect.moveLeft(rect.left() - (width() - pixmap()->width()) / 2.0);
        rect.moveTop(rect.top() - (height() - pixmap()->height()) / 2.0);
        setSelectedRect(rect);
    }

    rubberBand->hide();
}

void PageLabel::searchText(const QString &text)
{
    QList<QRectF> lists = PdfKit::GetInstance()->search(m_curPage,text);
    setSearchListRect(lists);
}

//void PageLabel::paintEvent(QPaintEvent *event)
//{
//    QLabel::paintEvent(event);//先调用父类的paintEvent为了显示'背景'!!!
//    QPainter painter(this);
//    painter.drawImage(0,0,mimage);
//}
