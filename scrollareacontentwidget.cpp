#include "scrollareacontentwidget.h"
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include "pdfkit.h"
#include "noteitemwidget.h"

ScrollAreaContentWidget::ScrollAreaContentWidget(QWidget *parent) :
    QWidget(parent)
  , mpScrollArea((QScrollArea*)parent)
  , rubberBand(NULL)
  , dragPosition(QPoint())
  , mbPrepareFlag(false)
{
    mNoteManagerVec.clear();
    m_PainterPoly.clear();
}

ScrollAreaContentWidget::~ScrollAreaContentWidget()
{

}

void ScrollAreaContentWidget::prepareAddNoteItem(bool bPrepare)
{
    mbPrepareFlag = bPrepare;
    setCursor(Qt::CrossCursor);
}

void ScrollAreaContentWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    //文档是关闭状态时，return
    if (!PdfKit::GetInstance()->getDocOpenState())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int nScrollHeight = 0; //当前窗口的高度
    int nScrollYFlag = 0;  //当前坐标（画布）
    nScrollHeight = mpScrollArea->size().height(); //当前绘制的剩余窗口高度
    nScrollYFlag  = GetVScrollPos();

    // 当前绘制的页码
    int nPageNum;
    // 当前页码绘制的起始VPos
    int nPageVPos;
    // 当前页码的图片
    QImage img;
    QImage imageCopy;

    int nXDrawPos = 0;

    while (nScrollHeight > 0)
    {
        GetVPos(nScrollYFlag, nPageNum, nPageVPos);
        // 绘制当前页的高度
        int nRenderHeight = PdfKit::GetInstance()->getCurPageSize(nPageNum).height() + PAGE_SPACE - nPageVPos;

        imageCopy = PdfKit::GetInstance()->getCurPageImage(nPageNum);
        img = imageCopy.copy(0, nPageVPos,
                             PdfKit::GetInstance()->getCurPageSize(nPageNum).width(),
                             nRenderHeight);

//        QSize sz = m_ChildViewer->getDocWidgetSize();
////        int nDD = doc->size()->width();

//        nXDrawPos = (sz.width() - GetPageSize(nPageNum).width()) / 2;

        painter.drawImage(nXDrawPos, nScrollYFlag, img);

        nScrollHeight -= img.height();
        nScrollYFlag += img.height();
    }

    //painter.drawImage(50,100,QImage(":/img/star-icon.png"));

    //绘制批注提示框
    if (!m_PainterPoly.isEmpty())
    {
        QPainterPath path;
        painter.setBrush(Qt::yellow);
        painter.setPen(Qt::yellow);
        path.addPolygon(m_PainterPoly);
        painter.drawPath(path);
    }
}

int ScrollAreaContentWidget::GetVScrollPos()
{
    QScrollBar* pScrollBar = mpScrollArea->verticalScrollBar();
    int value = pScrollBar->value();
    qDebug()<<"QScrollBar value  = "<<value;
    return value;
}

bool ScrollAreaContentWidget::GetVPos(int nScrollPos, int &nPageNum, int &nPageVStartPos)
{
    int nPageCount = PdfKit::GetInstance()->getPageNum();

    for (int i = 0; i < nPageCount; i++)
    {
        if (nScrollPos < (PdfKit::GetInstance()->getCurPageSize(i).height() + PAGE_SPACE))
        {
            nPageNum = i;
            nPageVStartPos = nScrollPos;
            break;
        }
        nScrollPos -= (PdfKit::GetInstance()->getCurPageSize(i).height() + PAGE_SPACE);
    }

    return true;
}

void ScrollAreaContentWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
    {

        if (!mbPrepareFlag)
        {
            dragPosition = event->pos();
            if (!rubberBand)
                rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
            rubberBand->setGeometry(QRect(dragPosition, QSize()));
            rubberBand->show();
        }
    }
}

void ScrollAreaContentWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    if (NULL != rubberBand)
        rubberBand->setGeometry(QRect(dragPosition, event->pos()).normalized());
}

void ScrollAreaContentWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    if (!mbPrepareFlag)
    {
        if (!rubberBand->size().isEmpty()) {
            // Correct for the margin around the image in the label.
            QRectF rect = QRectF(rubberBand->pos(), rubberBand->size());

            qDebug()<<"rect x= "<<rect.x()<<"y:"<<rect.y()<<"width:"<<rect.width()<<"height:"<<rect.height();
            PdfKit::GetInstance()->setSelectedRect(rect.toRect());
            update();
            GetVScrollPos();
        }
         rubberBand->hide();
    }else
    {
        //添加批注
        mbPrepareFlag = false;
        setCursor(Qt::ArrowCursor);
        addNoteItem(event->pos());
    }


}

void ScrollAreaContentWidget::addNoteItem(const QPoint& pt) //添加批注
{
    NoteItemWidget *pItem = new NoteItemWidget(this);
    pItem->setProperty("Point",QVariant(QPoint(pt)));
    connect(pItem,SIGNAL(eCloseNoteItem()),this,SLOT(onCloseNoteItem()));
    connect(pItem,SIGNAL(ePreparePaint(bool,const QPoint&)),this,SLOT(onPreparePaintItem(bool,const QPoint&)));
    mNoteManagerVec.append(pItem);
    pItem->move(pt);
    pItem->show();
}

void ScrollAreaContentWidget::onCloseNoteItem()
{
    QObject * obj = this->sender();
    if (obj == NULL || mNoteManagerVec.isEmpty())
        return;

    for (int i = 0; i<mNoteManagerVec.size();i++)
    {
        if (obj == mNoteManagerVec.at(i))
        {
            obj->deleteLater();
            mNoteManagerVec.remove(i);
            break;
        }
    }
}

void ScrollAreaContentWidget::onPreparePaintItem(bool bFlag,const QPoint& pt)
{
    m_PainterPoly.clear();

    if (bFlag)
    {
        m_PainterPoly.append(pt);
        NoteItemWidget* pitem = (NoteItemWidget*)this->sender();
        QPoint pt1 = pitem->geometry().topLeft();
        pt1 += QPoint(2,2);
        QPoint pt2 = pt1;
        pt2.setY(pt2.y() + 20);
        m_PainterPoly.append(pt1);
        m_PainterPoly.append(pt2);
        m_PainterPoly.append(pt);
    }
    this->update();
}
