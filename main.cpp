#include <QApplication>
#include "com/command.h"
#include "slamtec/slamtec.h"
#include <QDebug>
#include <io/map_reader_mapper.h>
#include "test.h"
using namespace NJRobot;
int main(int argc, char *argv[])
{
    MapReadMapper aa;
    qDebug()<<aa.isMapLoaded();
    QApplication a(argc, argv);
    Command *m_msgCom =new Command();
    m_msgCom->startCommand();
    slamTec *m_slamTec =new slamTec();
    test *t = new test();
    qRegisterMetaType<LaserScan>("LaserScan");
    QObject::connect(m_slamTec,SIGNAL(sendLaserScan(LaserScan)),t,SLOT(addData(LaserScan)));
    t->show();
	return a.exec();
}
