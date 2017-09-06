#ifndef QSEARCHTHREAD_H
#define QSEARCHTHREAD_H

#include <QThread>

namespace Ui {
class QSearchThread;
}

class QSearchThread : public QThread
{
    Q_OBJECT

public:
    explicit QSearchThread(QObject *parent = 0);
    ~QSearchThread();

protected:
    void run();

private:

};

#endif // QSEARCHTHREAD_H
