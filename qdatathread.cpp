#include "qdatathread.h"
#include <qDebug>
#include <QTime>

QDataThread::QDataThread(QObject *parent) :
    QThread(parent)
  , mpData(NULL)
{
    mpData = new QDataProvider();
    mpData->setParent(this);
    connect(mpData,SIGNAL(onFinishData(const QString &)),this->parent(),SLOT(openFileAuto(const QString &)));

    qDebug()<<"QDataThread::QDataThread id:"<<(int)currentThreadId();
}
void QDataThread::run()
{
    qDebug()<<"loadData current thread id:"<<(int)currentThreadId();
    QTime time;
    time.start();
    mpData->loadData();
    qDebug()<<"QDataThread::run() time elapsed:"<<time.elapsed();
}
