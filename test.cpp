#include "test.h"
#include "ui_test.h"

test::test(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::test)
{
    ui->setupUi(this);
    m_scene = new DiagramScene(ui->graphicsView,this);
    m_scene->setSceneRect(QRectF(-2000, -2000, 4000, 4000));
    ui->graphicsView->setScene(m_scene);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->graphicsView->setAcceptDrops(true);

}

test::~test()
{
    delete ui;
}

void test::addData(LaserScan laser){
    static int ii =0 ;
    if(ii == 0 ){
        m_scene->aa();
        ii++;
    }
    m_scene->addData(laser);
}
