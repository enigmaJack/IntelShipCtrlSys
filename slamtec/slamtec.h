#ifndef SLAMTEC_H
#define SLAMTEC_H

#include <QThread>
#include "rplidar.h" //RPLIDAR standard sdk, all-in-one header
#include "common/robot_type.h"

using namespace rp::standalone::rplidar;
using namespace NJRobot;

class slamTec : public QThread
{
    Q_OBJECT
public:
    explicit slamTec(QObject *parent = 0);
    ~slamTec();

    void run();
signals:
    void sendLaserScan(LaserScan laser);
public slots:
    
private:
    bool checkRPLIDARHealth(RPlidarDriver * drv);

    bool m_isStop;
    QString m_comPost;
    RPlidarDriver * m_drv;
    rplidar_response_device_info_t m_devinfo;
};

#endif // SLAMTEC_H
