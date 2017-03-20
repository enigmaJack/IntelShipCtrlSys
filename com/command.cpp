#include "command.h"
#include <QSettings>
#include <QDebug>

#define CONFIG_PATH "./config.ini"

Command::Command(QObject *parent) :
	QThread(parent)
{
	sendPort = NULL;
	getPort = NULL;
	QSettings *config = new QSettings(CONFIG_PATH, QSettings::IniFormat);
	QString send = config->value("System/SendCom").toString();
	QString get = config->value("System/GetCom").toString();
	delete config;
    struct PortSettings myComSettings = {BAUD115200,
				DATA_8,
				PAR_NONE,
				STOP_1,
				FLOW_OFF,
                100};
    sendPort = new QextSerialPort(send,myComSettings,QextSerialPort::Polling);
    getPort = new QextSerialPort(get,myComSettings,QextSerialPort::Polling);
	if(sendPort->open(QIODevice::ReadWrite)){
		qDebug()<<("sendPort open s!");
	}else{
		qDebug()<<(tr("sendPort open f!  com=%1").arg(send));
	}
	if(getPort->open(QIODevice::ReadWrite)){
		qDebug()<<("getPort open s!");
	}else{
		qDebug()<<(tr("getPort open f!  com=%1").arg(get));
	}

	m_isStop = true;
	m_isStartTest = false;
    m_cmdType = Motor;
	m_sendNum = 0;
}
Command::~Command()
{
	m_isStop = true;
	if(sendPort != NULL)
	{
		sendPort->close();
	}
	delete sendPort;
	if(getPort != NULL)
	{
		getPort->close();
	}
	delete getPort;
}

void Command::startCommand()
{
    m_isStop = false;
    start();
}

void Command::run()
{
	while (!m_isStop) {
        unsigned char cmd;
		QList<QByteArray> recvList;
		QByteArray recvData = getPort->readAll();
		if (recvData.size() <=0)
        {
			msleep(50);
			continue;
		}
        qDebug()<<("GET-data:"+QString(recvData));
		recvList =  splitRecvPackage(recvData);
		foreach(QByteArray date,recvList){
			int rec =-10;
            //qDebug()<<("SPLIT-data:"+QString(date));
            QByteArray odate = date;
			rec = analysisRecvPackage(cmd,date);
			if(0 > rec)
			{
				msleep(50);
				continue;
			}
			switch (m_cmdType) {
			case Idle:
				break;
			case Motor:
				if(cmd == 'M'){
                    //qDebug()<<("SEND-ACK:");
					QByteArray ack;
					ack.push_back(0x06);
					getPort->write(ack);
                    //qDebug()<<("SEND-data:");
                    sendPort->write(odate);
				}
				break;
			default:
				break;
			}
        }/*
		QList<QByteArray> sendRecvList;
		QByteArray sendRecvData = sendPort->readAll();
		if (sendRecvData.size() <=0)
		{
			msleep(50);
			continue;
		}
		sendRecvList =  splitRecvPackage(sendRecvData);
		foreach(QByteArray date,recvList){
			int rec =-10;
			rec = analysisRecvPackage(cmd,date);
		}
        msleep(10);*/
	}
}

int Command::closeComPort()
{
	m_isStop = true;
	if(sendPort != NULL)
	{
		sendPort->close();
		sendPort = NULL;
	}
	if(getPort != NULL)
	{
		getPort->close();
		getPort = NULL;
	}
}

Command::CMDTYPE Command::getCMDtype()
{
	return m_cmdType;
}

unsigned int Command::sumCheck(unsigned char *buf, int len)
{
	unsigned int checkValue  =0;
	for (int i = 0; i < len; i ++)
	{
		checkValue += buf[i];
	}
	qDebug()<<"checkValue:"<<checkValue;
	return checkValue;
}

int Command::groupSendPackage(unsigned char cmd,QByteArray &dst, QByteArray &srcData)
{
	dst.push_back(0x02);
	dst.push_back(cmd);
	dst.push_back(srcData);
	dst.push_back(0x03);
	unsigned int check = sumCheck((unsigned char*)dst.data(),dst.size());
	check &= 0xFF;
	qDebug()<<"check:"<<check;
	char ch=(unsigned char )(check >> 4);
	char cl=(unsigned char )check&0x0F;
	if(ch<10) //校验和高位
	{
		ch+=0x30;
	}
	else
	{
		ch+=0x37;
	}

	if(cl<10) //校验和高位
	{
		cl+=0x30;
	}
	else
	{
		cl+=0x37;
	}
	dst.push_back(ch);
	dst.push_back(cl);
	dst.push_back(0x0d);
	dst.push_back(0x0a);
	return dst.size();
}

QList<QByteArray> Command::splitRecvPackage(QByteArray &data)
{
	QList<QByteArray> dataList;
	QByteArray key;
	key.push_back(0x0D);
	key.push_back(0x0A);
	if(data.size()<=0)
	{
		qDebug()<<("data is NULL!");
		return dataList;
	}

    for(int i = 0;i<100;i++){
		int index = data.indexOf(key,i);
		if(index >0){
			dataList.push_back(data.mid(i,index+2-i));
            i=index+1;
		}else{
			break;
		}
	}
	return dataList;
}

int Command::analysisRecvPackage(unsigned char &cmd,QByteArray &data)
{
	if(data.size()<=0)
	{
		qDebug()<<("data is NULL!");
		return -5;
	}

	if(data.at(0) == 0x06)
	{
		qDebug()<<("data is ACK!");
		return 0;
	}

	if(data.at(0) == 0x15)
	{
		qDebug()<<("need resend!");
		return 1;
	}

	if(!(data.contains(0x02)&&data.contains(0x03)&&data.contains(0x0d)&&data.contains(0x0a)))
	{
		qDebug()<<("Command incomplete!");
		return -4;
	}

	unsigned char header = data.at(0);
	cmd  =  data.at(1);
	unsigned char tail1 = data.at(data.size() -2);
	unsigned char tail2 = data[data.size() -1];
	unsigned int recvCheck  = (data.at(data.size() -3) &0x0f)|((data.at(data.size() -4)&0x0f) << 4);
	if (header != 0x02)
	{
		qDebug()<<("header !=");
		return -1;
	}

	if (tail1 != 0x0D || tail2 != 0x0A)
	{
		qDebug()<<("tail !=");
		return -2;
	}
	unsigned int check = sumCheck((unsigned char*)data.data(),data.size() - 4);
	check &= 0xff;
	char ch=(unsigned char )(check >> 4);
	char cl=(unsigned char )check&0x0F;
	if(ch<10) //校验和高位
	{
		ch+=0x30;
	}
	else
	{
		ch+=0x37;
	}

	if(cl<10) //校验和高位
	{
		cl+=0x30;
	}
	else
	{
		cl+=0x37;
	}
	check = (cl &0x0f)|((ch &0x0f) << 4);
	if (check != recvCheck)
	{
		qDebug()<<("check !=");
		return -3;
	}

    data =  data.mid(2,data.size()-7);

	return 0;
}
