#ifndef TEST_H
#define TEST_H

#include <QWidget>
#include "display/diagramscene.h"

namespace Ui {
class test;
}

class test : public QWidget
{
    Q_OBJECT
    
public:
    explicit test(QWidget *parent = 0);
    ~test();
public slots:
    void addData(LaserScan laser);
private:
    Ui::test *ui;
    DiagramScene *m_scene;
};

#endif // TEST_H
