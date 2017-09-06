#ifndef SCROLLAREACONTENTWIDGET_H
#define SCROLLAREACONTENTWIDGET_H

#include <QWidget>
#include <QRubberBand>

namespace Ui {
class ScrollAreaContentWidget;
}

class QScrollArea;
class NoteItemWidget;
class ScrollAreaContentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScrollAreaContentWidget(QWidget *parent = 0);
    ~ScrollAreaContentWidget();
    void prepareAddNoteItem(bool bPrepare = true);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    int GetVScrollPos();
    bool GetVPos(int nScrollPos, int &nPageNum, int &nPageVStartPos);
    void addNoteItem(const QPoint& pt); //添加批注

private slots:
    void onCloseNoteItem();
    void onPreparePaintItem(bool bFlag,const QPoint& pt);

private:
    QScrollArea * mpScrollArea;
    QRubberBand  *rubberBand;
    QPoint        dragPosition;
    bool          mbPrepareFlag;
    QVector<NoteItemWidget*>   mNoteManagerVec;
    QPolygonF     m_PainterPoly;

};

#endif // SCROLLAREACONTENTWIDGET_H
