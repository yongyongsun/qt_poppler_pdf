#include "qdataprovider.h"
#include "pdfkit.h"
#include "QPrinter"
#include "QPainter"
#include <QDebug>
#include <QDir>
#include <Shlobj.h>

bool VecCompare(const DATA &data1,const DATA &data2)
{
    return data1.type < data2.type;
}

QDataProvider::QDataProvider()
{
    mstrAppDataPath = localAppDataPath();
    qDebug()<<"QDataProvider::QDataProvider(),localAppDataPath = "<<mstrAppDataPath;

    //add test 初始化打印设备时间
}

QDataProvider::~QDataProvider()
{

}

void QDataProvider::loadData()
{
    //clear data
    clearTempFiles();
    //add todo
    m_MapPagesInfo.clear();

    //测试第一页数据
    FontInfo info1;
    info1.color = QColor(255,200,200);
    info1.fontSize = 20;
    DATA data1;
    data1.type = DataSignature;
    data1.fontInfo = info1;
    data1.pPoint = QPoint(10,10);
    data1.data = QVariant(QString("111111111"));

    DATA data2;
    data2.type = DataImage;
    data2.fontInfo = info1;
    data2.pPoint = QPoint(50,50);
    data2.data = QVariant(QString("2222222"));

    DATA data3;
    data3.type = DataText;
    data3.fontInfo = info1;
    data3.pPoint = QPoint(50,150);
    data3.data = QVariant(QString("33333333"));

    DATA data4;
    data4.type = DataText;
    data4.fontInfo = info1;
    data4.pPoint = QPoint(50,250);
    data4.data = QVariant(QString("444444444444"));

    DATA data5;
    data5.type = DataImage;
    data5.fontInfo = info1;
    data5.pPoint = QPoint(50,350);
    data5.data = QVariant(QString("555555555"));

    vecPage page1;
    page1.append(data1);
    page1.append(data2);
    page1.append(data3);
    page1.append(data4);
    page1.append(data5);

    qStableSort(page1.begin(),page1.end(),VecCompare);
    m_MapPagesInfo.insert(0,page1);


    //数据读入pdf缓存
    readToPdf();

    emit onFinishData(mstrAppDataPath + FILENAME);
}

void QDataProvider::clearTempFiles()
{
    if (mstrAppDataPath.isEmpty())
        return;

    QTime time;
    time.start();

    QDir dir(mstrAppDataPath);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        QString str = fileInfo.filePath();
        bool bFlag = QFile::remove(str);
        qDebug() << "--------clearTempFiles: remove file" << str << " state = "<<bFlag;
    }

    qDebug()<<"clearTempFiles time elapsed:"<<time.elapsed();
}

bool QDataProvider::readToPdf()
{
    if (m_MapPagesInfo.size() == 0)
        return false;

    //数据读入pdf缓存
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(mstrAppDataPath + FILENAME);
    QPainter painter_text;
    painter_text.begin(&printer);

    MapPages::const_iterator i = m_MapPagesInfo.constBegin();
    while (i != m_MapPagesInfo.constEnd())
     {
         qDebug()<<"------------reading page = "<<i.key();
         vecPage vec = i.value();
         for(int j = 0; j<vec.size(); ++j)
         {
            DATA data = vec.at(j);
            if (data.type == DataText)//文本
            {
                painter_text.drawText(data.pPoint, data.data.toString());

            }else if (data.type == DataImage || data.type == DataSignature)//图片和签章
            {
                //painter_text.drawImage(data.pPoint, data.data.to);

            }else if (data.type == DataComment)//批注
            {
                //add todo
            }
         }

         ++i;
         if (i != m_MapPagesInfo.constEnd())
            printer.newPage();  //生成新的一页，可继续插入
     }

    painter_text.end();
    return true;
}

QString QDataProvider::localAppDataPath()
{
    wchar_t m_lpszDefaultDir[MAX_PATH];
    wchar_t szDocument[MAX_PATH] = {0};
    memset(m_lpszDefaultDir, 0, _MAX_PATH);

    LPITEMIDLIST pidl = NULL;
    SHGetSpecialFolderLocation(NULL,   CSIDL_LOCAL_APPDATA,   &pidl);
    if   (pidl && SHGetPathFromIDList(pidl, szDocument))
    {
        GetShortPathName(szDocument,m_lpszDefaultDir, _MAX_PATH);
    }

    wcscat(m_lpszDefaultDir,L"\\");
    wcscat(m_lpszDefaultDir,PDFVIEWER);

    if ( _waccess(m_lpszDefaultDir, 0) == -1 )
    {
        _wmkdir(m_lpszDefaultDir);
    }

    QString str = QString::fromStdWString(m_lpszDefaultDir);
    return str;
}
