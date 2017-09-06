#include "pdfkit.h"
#include <QMatrix>
#include <QDebug>
#include <QPainter>
#include "qdataprovider.h"

PdfKit * PdfKit::m_pInstance=NULL;
PdfKit::PdfKit(QWidget *parent)
    : QWidget(parent)
    , mPDoc(NULL)
    , scaleFactor(1.0)
    , bDocStatus(false)
    , mStrSearchText("")
    , m_SelectedRect(QRect())
    , m_pDataProvider(NULL)
{

}

bool PdfKit::initLoad(const QString &filePath)
{
    Poppler::Document *oldDocument = mPDoc;

    mPDoc = Poppler::Document::load(filePath);
    if (mPDoc)
    {
        if (NULL != oldDocument)
        {
            delete oldDocument;
            oldDocument = NULL;
        }

        mPDoc->setRenderHint(Poppler::Document::Antialiasing);
        mPDoc->setRenderHint(Poppler::Document::TextAntialiasing);
    }else
    {
        bDocStatus = false;
    }
    bDocStatus = true;

    return bDocStatus;
}


int PdfKit::getPageNum() //获取总页数
{
    int nNumCount = mPDoc->numPages();
    return nNumCount > 0 ? nNumCount: 0;
}
Poppler::Page * PdfKit::getPage(int nPage)//获取指定页
{
    Poppler::Page * pPage = mPDoc->page(nPage);
    return pPage;
}

QImage PdfKit::getCurPageImage(int npage)
{
    Poppler::Page * pPage = mPDoc->page(npage);
    QImage image = pPage->renderToImage(scaleFactor * physicalDpiX(), scaleFactor * physicalDpiY());
    //图片合成
    //1>:选择区域文本高亮
    if (m_SelectedRect.isValid())
    {
        //计算当前页所在坐标
        int nPageNumStartY = getSumPageSize(npage).height();
        QRect pageRect = QRect(QPoint(0,nPageNumStartY),image.rect().size());
        if (pageRect.intersects(m_SelectedRect))
        {
            //计算相交的矩形相对位置
            QRect rect = pageRect.intersected(m_SelectedRect);
            QRect Rects = QRect(rect.x(),rect.y() - nPageNumStartY,rect.width(),rect.height());
            qDebug()<<"---xiangjiao rect x= "<<Rects.x()<<"y:"<<Rects.y()
                   <<"width:"<<Rects.width()<<"height:"<<Rects.height();

            QRect relativeRect = matrix().inverted().mapRect(Rects);
            QList<QRectF> list = PdfKit::GetInstance()->selectedText(npage,relativeRect);
            int size = list.size();
            if (size != 0)
            {
                QPainter painter;
                painter.begin(&image);
                for (int i = 0; i< size;i++)
                {
                    QRect highlightRect = matrix().mapRect(list.at(i)).toRect();
                    highlightRect.adjust(-2, -2, 2, 2);
                    painter.fillRect(highlightRect, QColor(50, 0, 0, 32));
                }

                painter.end();
            }

        }
    }

    //2>:搜索文本高亮
    QList<QRectF> lists = search(npage,mStrSearchText);
    if (!lists.isEmpty())
    {
        QPainter painters;
        painters.begin(&image);
        for (int i = 0; i<lists.size();i++)
        {
            QRect highlightRect = matrix().mapRect(lists.at(i)).toRect();
            highlightRect.adjust(-2, -2, 2, 2);
            painters.fillRect(highlightRect, QColor(255, 255, 0, 100));
        }

        painters.end();
    }
    return image;
}

QList<QRectF> PdfKit::search(int ncurPage,const QString &text)
{
    QList<QRectF> list;
    list.clear();
    if (mPDoc == NULL || text.isEmpty())
        return list;

    Poppler::Page * pPage = mPDoc->page(ncurPage);
    list = pPage->search(text,Poppler::Page::IgnoreCase);

    return list;
}

QList<QRectF> PdfKit::selectedText(int ncurPage,const QRectF &rect)
{

     QString text11 = mPDoc->page(ncurPage)->text(rect);
     qDebug()<<"output text11 = "<<text11;

    QList<QRectF> listSelected;
    QString text;
    bool hadSpace = false;
    QPointF center;
    foreach (Poppler::TextBox *box, mPDoc->page(ncurPage)->textList()) {
        if (rect.intersects(box->boundingBox())) {
            if (hadSpace)
                text += " ";
            if (!text.isEmpty() && box->boundingBox().top() > center.y())
                text += "\n";
            text += box->text();
            hadSpace = box->hasSpaceAfter();

//            if (box->nextWord() != NULL)
//                text += box->nextWord()->text();


            center = box->boundingBox().center();
            listSelected.append(box->boundingBox());
        }
    }

    qDebug()<<"output text = "<<text;
    if (!text.isEmpty())
        emit textSelected(text);

    return listSelected;
}

QSize PdfKit::getCurPageSize(int npage)
{
    Poppler::Page * pPage = mPDoc->page(npage);
    QSize size =  pPage->pageSize();
    size.setWidth(scaleFactor * size.width()* physicalDpiX()/72);
    size.setHeight(scaleFactor * size.height()* physicalDpiY()/72);
    return size;
}

qreal PdfKit::scale() const
{
    return scaleFactor;
}

void PdfKit::Setscale(qreal index)
{
    if (scaleFactor != index)
    {
        scaleFactor = index;
    }
}
QMatrix PdfKit::matrix() const
{
    return QMatrix(scaleFactor * physicalDpiX() / 72.0, 0,
                   0, scaleFactor * physicalDpiY() / 72.0,
                   0, 0);
}

bool PdfKit::getDocOpenState() const
{
    return bDocStatus;
}

void PdfKit::SetSearchText(const QString& str)
{
    mStrSearchText = str;

    startSearchList(); //搜索listwidget数据添加
}

void PdfKit::startSearchList()
{
    int nNumCount = mPDoc->numPages();
    QVector<SearchData> vecPos;
    vecPos.clear();
    for (int i = 0; i < nNumCount; i++)
    {
        QList<Poppler::TextBox*> listTextboxs = mPDoc->page(i)->textList();
        QList<QRectF> lists = search(i,mStrSearchText);
        for (int j = 0; j<lists.size();j++)
        {
            SearchData data;
            foreach (Poppler::TextBox *box, listTextboxs)
            {
                if (lists.at(j).intersects(box->boundingBox()))
                {
                    data.text = box->text();
                    break;
                }
            }
            //计算需要滚动条移动的位移，页码*页高 + 当前页位置
            data.pos = getSumPageSize(i).height() + lists.at(j).y();
            vecPos.append(data);
        }



    }

    emit eSearchData(vecPos);
}

void PdfKit::setSelectedRect(const QRect& rect)
{
    m_SelectedRect = rect;
}

QSize PdfKit::getSumPageSize(int nSumPage)
{
    QSize sumSize = QSize();
    int nTotileNum = getPageNum();
    int nsum = nSumPage > nTotileNum ? nTotileNum : nSumPage;
    for (int i = 0; i<nsum; i++)
    {
        Poppler::Page * pPage = mPDoc->page(i);
        QSize size =  pPage->pageSize();
        size.setWidth(scaleFactor * size.width()* physicalDpiX()/72);
        size.setHeight(scaleFactor * size.height()* physicalDpiY()/72);

        if (sumSize.isEmpty())
        {
            sumSize = size;
            sumSize.rheight() += PAGE_SPACE;
        }
        else
            sumSize.rheight() += (size.height() + PAGE_SPACE);
    }

    return sumSize;
}
