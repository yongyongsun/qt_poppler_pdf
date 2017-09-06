#ifndef NoteItemWidget_H
#define NoteItemWidget_H

#include <QWidget>

namespace Ui {
class NoteItemWidget;
}

class NoteItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NoteItemWidget(QWidget *parent = 0);
    ~NoteItemWidget();
    QString getNoteItemText();

signals:
    void eCloseNoteItem();
    void ePreparePaint(bool,const QPoint&);
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void onclosedClicked();

private:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent (QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
private:
    //辅助函数
    int calcPosition(const QPoint& pt);
    void setCursorType(int value);
    QPoint adjustPoint(const QPoint& pt);
private:
    Ui::NoteItemWidget           *ui;
    bool                    m_bLeftMouseButtonPressed;
    int                     m_lastPosition;
    QPoint                  m_ptLast;
    Qt::CursorShape         m_currentCursor;
};

#endif // NoteItemWidget_H
