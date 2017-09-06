#ifndef QDATATHREAD_H
#define QDATATHREAD_H
#include <QThread>
#include "qdataprovider.h"

class QDataThread : public QThread
{
    Q_OBJECT
public:
    explicit QDataThread(QObject *parent = 0);

protected:
    void run();

private:
    QDataProvider *mpData;

};

#endif // QDATATHREAD_H
