#ifndef COMMAND_H
#define COMMAND_H

#include <QThread>
#include <QTimer>
#include <QList>
#include <QByteArray>
#include "qextserialport.h"
#include "qextserialenumerator.h"

class Command : public QThread
{
Q_OBJECT
public:
	enum CMDTYPE{Idle=0,Motor};
	explicit Command(QObject *parent = 0);
    ~Command();

    int closeComPort();
    void startCommand();
    CMDTYPE getCMDtype();
	void run();

signals:
public slots:

private:
	QList<QByteArray> splitRecvPackage(QByteArray &data);
	int analysisRecvPackage(unsigned char &cmd,QByteArray &data);
	int groupSendPackage(unsigned char cmd,QByteArray &dst, QByteArray &srcData);
	unsigned int sumCheck(unsigned char *buf, int len);
public:
    QextSerialPort *sendPort;
    QextSerialPort *getPort;
    bool m_isStop;
	bool m_isStartTest;
    int m_sendNum;
	CMDTYPE m_cmdType;
};

extern Command cmdFunc;
#endif // COMMAND_H
