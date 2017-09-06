#ifndef QDATAPROVIDER_H
#define QDATAPROVIDER_H

#include <QObject>
#include <QFont>
#include <QPoint>
#include <QPixmap>
#include <QVector>
#include <QMap>
#include <QVariant>
#include <QPrinter>

#define PDFVIEWER  L"pdfviewer"
#define FILENAME    "\\temp.pdf"
typedef enum DataType{
    DataText=0,         //文本
    DataImage,          //图片
    DataSignature,      //签章
    DataComment,        //批注
}DataType;

typedef struct FontInfo{
    QColor color;
    int fontSize;
}FontInfo;


typedef struct DATA{
    DataType  type;     //数据类型
    QPoint    pPoint;   //坐标
    FontInfo  fontInfo;
    QVariant  data;    //图片,文本
}DATA;

typedef QVector<DATA> vecPage; //某一页数据
typedef QMap<int,vecPage> MapPages; //总数据
class QDataProvider : public QObject
{
    Q_OBJECT

public:
    explicit QDataProvider();
    ~QDataProvider();
    void loadData();
    void clearTempFiles();

signals:
    void onFinishData(const QString &);

private:
    bool readToPdf();
    QString localAppDataPath();

private:
    MapPages  m_MapPagesInfo;
    QString   mstrAppDataPath;
    QPrinter  printer;

};

#endif // QDATAPROVIDER_H
