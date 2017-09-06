#ifndef PDFKIT_H
#define PDFKIT_H

#include <QWidget>
#include <poppler-qt5.h>
#include <QPaintDevice>

#define PAGE_SPACE 10

class QDataProvider;

typedef struct SearchData{
    int pos;
    QString text;
}SearchData;

typedef QVector<SearchData> vecSearchData;
class PdfKit : public QWidget
{
    Q_OBJECT
public:

    static PdfKit * GetInstance()
    {
        if(m_pInstance==NULL)
            m_pInstance=new PdfKit();
        return m_pInstance;
    }

    bool initLoad(const QString &filePath);
    int getPageNum(); //获取总页数
    Poppler::Page * getPage(int nPage);//获取指定页
    QImage getCurPageImage(int npage);
    QList<QRectF> search(int ncurPage,const QString &text);
    QList<QRectF> selectedText(int ncurPage,const QRectF &rect);
    QSize getCurPageSize(int npage);
    qreal scale() const;
    QMatrix matrix() const;
    void Setscale(qreal index);
    bool getDocOpenState() const;
    void SetSearchText(const QString& str);
    void setSelectedRect(const QRect& rect);
    QSize getSumPageSize(int nSumPage); //获取从0到当前页size

signals:
    void textSelected(const QString&);
    void eSearchData(const vecSearchData &);

private:
    explicit PdfKit(QWidget *parent = 0);
    void startSearchList();



private:
    static PdfKit * m_pInstance;
    Poppler::Document* mPDoc;
    qreal scaleFactor;
    bool  bDocStatus;
    QString mStrSearchText;
    QRect   m_SelectedRect;
    QDataProvider *m_pDataProvider;


};

#endif // PDFKIT_H
