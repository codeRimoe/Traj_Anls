#include "Save.h"
#include "ui_save.h"

#include "mainwindow.h"

Save::Save(MainWindow *w_main, QWidget *parent) :
    QDialog(parent),
    mw_main(w_main),
    ui(new Ui::Save)
{
    ui->setupUi(this);
    setWindowOpacity(0.9);
    this->setWindowFlags(Qt::FramelessWindowHint);
    connect(ui->SaveButton,SIGNAL(clicked(bool)),this,SLOT(save_b()));
    connect(ui->CanButton,SIGNAL(clicked(bool)),this,SLOT(cancel_b()));
}

Save::~Save()
{
    delete ui;
}

//实现窗口移动
void Save::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        mb_move = true;
        mqp_startPoint =  event->globalPos();//记录鼠标的世界坐标.
        mqp_windowPoint = this->frameGeometry().topLeft();//记录窗体的世界坐标.
    }
}

void Save::mouseMoveEvent(QMouseEvent *event){
    if (mb_move)
        if (event->buttons() & Qt::LeftButton){
            //移动中的鼠标位置相对于初始位置的相对位置.
            QPoint _relativePos = event->globalPos() - mqp_startPoint;
            //然后移动窗体即可.
            this->move(mqp_windowPoint + _relativePos );
        }
}

void Save::mouseReleaseEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton)
        //改变移动状态.
        mb_move = false;
}


void Save::save_b(){
    // gml, gj, shp, txt
    int _res=0;
    if (this->ui->cB1_txt->isChecked())
        _res += 1;
    if (this->ui->cB2_shp->isChecked())
        _res += 10;
    if (this->ui->cB3_gj->isChecked())
        _res += 100;
    if (this->ui->cB4_gml->isChecked())
        _res += 1000;
    QString savename = ui->saveName->text();
    this->close();
    this->mw_main->save_res(_res, savename);
}

void Save::cancel_b(){
    this->close();
}
