#include "NoteItemWidget.h"
#include "ui_NoteItemWidget.h"

#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
namespace
{
    const int g_padding = 4;

    int helperCalcPosition(int pos, int range)
    {
        return (pos < g_padding) ? 0 : ((pos > (range - g_padding)) ? 2 : 1);
    }
}

NoteItemWidget::NoteItemWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NoteItemWidget)
    , m_bLeftMouseButtonPressed(false)
    , m_lastPosition(11)
    , m_currentCursor(Qt::ArrowCursor)
{
    ui->setupUi(this);
    setMouseTracking(true);
    ui->widget->setMouseTracking(true);
    setCursor(m_currentCursor);

   // setStyleSheet("QWidget{border: 2px solid #FF00FF; border-radius: 5px;}");
    connect(ui->btnClose,SIGNAL(clicked()),this,SLOT(onclosedClicked()));
}

NoteItemWidget::~NoteItemWidget()
{

}

void NoteItemWidget::enterEvent(QEvent *e)
{
    QWidget::enterEvent(e);
    qDebug()<<"鼠标移入";
    emit ePreparePaint(true,this->property("Point").toPoint());
}

void NoteItemWidget::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);
    qDebug()<<"鼠标移出";
    emit ePreparePaint(false,this->property("Point").toPoint());
}

void NoteItemWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);   // 反锯齿;
    painter.setBrush(QBrush(Qt::yellow));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter.drawRoundedRect(rect, 5, 5);
//    //也可用QPainterPath 绘制代替 painter.drawRoundedRect(rect, 15, 15);
//    {
//        QPainterPath painterPath;
//        painterPath.addRoundedRect(rect, 15, 15);
//        p.drawPath(painterPath);
//    }
    QWidget::paintEvent(event);
}

int NoteItemWidget::calcPosition(const QPoint& pt)
{
    int row = helperCalcPosition(pt.y(), rect().height());
    int col = helperCalcPosition(pt.x(), rect().width());
    return row * 10 + col;
}

void NoteItemWidget::mousePressEvent(QMouseEvent* event)
{
    m_bLeftMouseButtonPressed = true;
    m_ptLast = event->globalPos();
    m_lastPosition = calcPosition(event->pos());
    event->accept();
}

void NoteItemWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if(m_bLeftMouseButtonPressed)
        m_bLeftMouseButtonPressed = false;
    event->accept();
}

void NoteItemWidget::mouseMoveEvent(QMouseEvent* event)
{
    setCursorType(calcPosition(event->pos()));
    if(m_bLeftMouseButtonPressed)
    {
        QPoint ptNew = event->globalPos();
        ptNew -= m_ptLast;
        if(11 == m_lastPosition) //拖动
        {
            ptNew += pos();
            move(adjustPoint(ptNew));
        }
        else //调整大小
        {
            QRect rectWindow = geometry();
            switch(m_lastPosition)
            {
            case 00:
                rectWindow.setTopLeft(rectWindow.topLeft() + ptNew);
                break;
            case 02:
                rectWindow.setTopRight(rectWindow.topRight() + ptNew);
                break;
            case 20:
                rectWindow.setBottomLeft(rectWindow.bottomLeft() + ptNew);
                break;
            case 22:
                rectWindow.setBottomRight(rectWindow.bottomRight() + ptNew);
                break;
            case 10:
                rectWindow.setLeft(rectWindow.left() + ptNew.x());
                break;
            case 12:
                rectWindow.setRight(rectWindow.right() + ptNew.x());
                break;
            case 01:
                rectWindow.setTop(rectWindow.top() + ptNew.y());
                break;
            case 21:
                rectWindow.setBottom(rectWindow.bottom() + ptNew.y());
                break;
            //case 11:
            default:
                Q_ASSERT(0);
            }
            setGeometry(rectWindow);
            //qDebug() << rectWindow.topLeft() << ", " << rectWindow.bottomRight();
        }
        m_ptLast  = event->globalPos();
    }
    event->accept();
}

QPoint NoteItemWidget::adjustPoint(const QPoint& pt)
{
    QPoint point = pt;
    qDebug() <<"mouseMoveEvent ptNew x = " << point.x()<<"y = "<<point.y();
    if (point.x() < 0)
        point.setX(0);
    else if (point.x() + this->width() > this->parentWidget()->width())
        point.setX(this->parentWidget()->width() - this->width());
    else if (point.y() < 0)
        point.setY(0);
    else if (point.y() + this->height() > this->parentWidget()->height())
        point.setY(this->parentWidget()->height() - this->height());

    return point;
}
void NoteItemWidget::setCursorType(int value)
{
    Qt::CursorShape cursor;

    switch(value)
    {
    case 00:
    case 22:
        cursor = Qt::SizeFDiagCursor;
        break;
    case 02:
    case 20:
        cursor = Qt::SizeBDiagCursor;
        break;
    case 10:
    case 12:
        cursor = Qt::SizeHorCursor;
        break;
    case 01:
    case 21:
        cursor = Qt::SizeVerCursor;
        break;
    case 11:
        cursor = Qt::ArrowCursor;
        break;
    default:
        Q_ASSERT(0);
        break;
    }
    if(cursor != m_currentCursor)
    {
        m_currentCursor = cursor;
        setCursor(m_currentCursor);
    }
}

QString NoteItemWidget::getNoteItemText()
{
    QString str = ui->textEdit->toPlainText();
    qDebug()<<"the note item text = "<<str;
    return str;
}
void NoteItemWidget::onclosedClicked()
{
    this->close();
}

void NoteItemWidget::closeEvent(QCloseEvent *event)
{
    emit eCloseNoteItem();
    QWidget::closeEvent(event);
}
