#include <QThread>

class MyThread : public QThread {	

public:
	MyThread(QObject *parent);
	~MyThread();

protected:
	void run();

private:
	bool _bFinished;
	
};

