#include "slamtec.h"
#include "common/common.h"
#include <QDebug>
#include <QSettings>

#define CONFIG_PATH "./config.ini"

#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

using namespace rp::standalone::rplidar;

using namespace NJRobot;

slamTec::slamTec(QObject *parent) :
    QThread(parent)
{
    QSettings *config = new QSettings(CONFIG_PATH, QSettings::IniFormat);
    m_comPost = config->value("System/SlamTec").toString();
    delete config;
    m_drv = RPlidarDriver::CreateDriver(RPlidarDriver::DRIVER_TYPE_SERIALPORT);

    if (!m_drv) {
        qDebug()<< "insufficent memory, exit";
    }


    // make connection...
    if (IS_FAIL(m_drv->connect(m_comPost.toLatin1(), 115200))) {
        qDebug() << "Error, cannot bind to the specified serial port %s.";
    }

    if (IS_FAIL(m_drv->getDeviceInfo(m_devinfo))) {
        qDebug() << "Error, cannot get device info.";
    }

    // print out the device serial number, firmware and hardware version number..
    QString info;
    info += ("RPLIDAR S/N: ");
    for (int pos = 0; pos < 16 ;++pos) {
        info += QString("%1").arg(m_devinfo.serialnum[pos],0,16);
    }

    qDebug() << info;

    qDebug() << QString("Firmware Ver: %d.%02d\n Hardware Rev: %d\n")
                .arg(m_devinfo.firmware_version>>8)
                .arg(m_devinfo.firmware_version & 0xFF)
                .arg((int)m_devinfo.hardware_version);



    // check health...
    if (!checkRPLIDARHealth(m_drv)) {
        qDebug() << "!checkRPLIDARHealth(drv)";
    }
    m_drv->startMotor();
    // start scan...
    m_drv->startScan();

    m_isStop = false;
    start();
}

slamTec::~slamTec()
{
    m_isStop = true;
}

bool slamTec::checkRPLIDARHealth(RPlidarDriver * drv)
{
    u_result     op_result;
    rplidar_response_device_health_t healthinfo;


    op_result = drv->getHealth(healthinfo);
    if (IS_OK(op_result)) { // the macro IS_OK is the preperred way to judge whether the operation is succeed.
        qDebug() << QString("RPLidar health status : %1").arg(healthinfo.status);
        if (healthinfo.status == RPLIDAR_STATUS_ERROR) {
            qDebug() << QString("Error, rplidar internal error detected. Please reboot the device to retry.");
            return false;
        } else {
            return true;
        }

    } else {
        qDebug() << QString("Error, cannot retrieve the lidar health code: %1").arg(op_result,0,16);
        return false;
    }
}

void slamTec::run()
{
    while (!m_isStop) {
        rplidar_response_measurement_node_t nodes[360*2];
        size_t   count = _countof(nodes);
        LaserScan laser;
        if (IS_OK(m_drv->grabScanData(nodes, count))) {
            m_drv->ascendScanData(nodes, count);
            for (int pos = 0; pos < (int)count ; ++pos) {
                Point p;
                p.x = nodes[pos].distance_q2/4.0f * cos(deg2rad((nodes[pos].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT)/64.0f)) / 1000.0;
                p.y = nodes[pos].distance_q2/4.0f * sin(deg2rad((nodes[pos].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT)/64.0f)) / 1000.0;
                laser.push_back(p);
            }
            emit sendLaserScan(laser);
        }
    }
}
