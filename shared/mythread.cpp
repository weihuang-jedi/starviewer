MyThread::MyThread(QObject *parent)
        : QThread(parent), _bFinished( false )
{
    start();
}

MyThread::~MyThread()
{
    _bFinished = true;
    wait();
}

void MyThread::run()
{
    while(! _bFinished)
    {
      //Do something
        QThread::msleep(100);
    }
}

