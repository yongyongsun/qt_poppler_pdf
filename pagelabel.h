#ifndef PAGELABEL_H
#define PAGELABEL_H

#include <QLabel>
#include <QRubberBand>

namespace Ui {
class PageLabel;
}
class Window;
class PageLabel : public QLabel
{
    Q_OBJECT

public:
    explicit PageLabel(QWidget *pWindow, QWidget *parent = 0);
    ~PageLabel();
    void ShowCurSacleImg();
    void setCurLabelPage(int nvalue);
    void searchText(const QString &text);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
//    void paintEvent(QPaintEvent *event) ;

private:
    void setSearchListRect(const QList<QRectF>& lists);
    void setSelectedRect(QRectF rect);

public slots:
    void onUpdateSearch();
    void onUpdateSelected();
    void onClear();

private:
    Window*       m_parent;
    QList<QRectF> m_listSearchRect;
    QRectF        m_SelectedRect;
    QRubberBand  *rubberBand;
    QPoint        dragPosition;
    int           m_curPage;
    QPixmap       m_oldpixMap; //保存原始pixmap，用于clear掉搜索和选择后恢复
    QPainter*     mpainter;
    QImage        mimage;

};

#endif // PAGELABEL_H
