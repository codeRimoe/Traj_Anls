#include "setting.h"
#include "ui_setting.h"

#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

#include <QDateTime>
#include <fstream>

#include "mainwindow.h"


Setting::Setting(MainWindow *mw, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Setting),
    mw_main(mw),
    mPara()
{
    ui->setupUi(this);
    setWindowOpacity(0.9);

    ButtonGroup.setExclusive(true);
    ButtonGroup.addButton(ui->form_rad_1,1);
    ButtonGroup.addButton(ui->form_rad_2,2);
    ButtonGroup.addButton(ui->form_rad_3,3);


    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setWindowModality(Qt::ApplicationModal);
    connect(ui->FileButton,SIGNAL(clicked(bool)),this,SLOT(openFile()));
    connect(ui->OpenButton,SIGNAL(clicked(bool)),this,SLOT(readFile()));
    connect(ui->check_OK,SIGNAL(clicked(bool)),this,SLOT(setPara()));
    connect(ui->check_Reset,SIGNAL(clicked(bool)),this,SLOT(reset()));
    connect(ui->check_cancel,SIGNAL(clicked(bool)),this,SLOT(closeWin()));
    connect(ui->TimePre,SIGNAL(clicked(bool)),this,SLOT(previewTime()));
}

Setting::~Setting(){
    delete ui;
}

//实现窗口移动
void Setting::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        mb_move = true;
        mqp_startPoint =  event->globalPos();//记录鼠标的世界坐标.
        mqp_windowPoint = this->frameGeometry().topLeft();//记录窗体的世界坐标.
    }
}

void Setting::mouseMoveEvent(QMouseEvent *event){
    if (mb_move)
        if (event->buttons() & Qt::LeftButton){
            //移动中的鼠标位置相对于初始位置的相对位置.
            QPoint _relativePos = event->globalPos() - mqp_startPoint;
            //然后移动窗体即可.
            this->move(mqp_windowPoint + _relativePos );
        }
}

void Setting::mouseReleaseEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton)
        //改变移动状态.
        mb_move = false;
}

struct Para_Set Setting::get_para(){
    return this->mPara;
}

void Setting::openFile(){

    QString _fileName = QFileDialog::getOpenFileName(
                this, tr("open data file"),
                "./", tr("Txt files(*.txt *.csv);;All files (*.*)"));

    if(_fileName.isEmpty()){
        QMessageBox _mesg;
        _mesg.warning(this,"Oops","Sorry, no file has been selected!");
        return;
    }
    else{
        ui->FileBox->setText(_fileName);
        this->mPara.mps_qsFileName = _fileName;

        this->ui->form_rad_1->setEnabled(true);
        this->ui->form_rad_2->setEnabled(true);
        this->ui->form_rad_3->setEnabled(true);
        this->ui->form_check_line1->setEnabled(true);
        this->ui->OpenButton->setEnabled(true);

        this->ui->DataTable->clear();
        ui->DataTable->setEnabled(false);
        ui->cB0_usrID->setEnabled(false);
        ui->cB1_time->setEnabled(false);
        ui->cB2_lng->setEnabled(false);
        ui->cB3_lat->setEnabled(false);

        if (_fileName.right(3) == "txt")
            this->ui->form_rad_2->click();
        else
            this->ui->form_rad_1->click();
        return;
    }
}

void Setting::readFile(){
    switch (this->ButtonGroup.checkedId()) {
    case 2:
        this->mPara.mps_cSeg = '\t';
        break;
    case 3:
        this->mPara.mps_cSeg = this->ui->form_rad_3txt->text().toStdString()[0];
        break;
    default:
        this->mPara.mps_cSeg = ',';
        break;
    }
    this->mPara.mps_bNoLine1=this->ui->form_check_line1->checkState();

    QFile file(this->mPara.mps_qsFileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox mesg;
        mesg.warning(this,"Oops","Sorry, the file can not be opened, please check the file!");
        return;
    }

    ui->cB0_usrID->clear();
    ui->cB1_time->clear();
    ui->cB2_lng->clear();
    ui->cB3_lat->clear();

    QTextStream _qts_inFile(&file);

    QStringList _qsl_head, _qsl_list;
    QString _qs_fileLine = _qts_inFile.readLine();

    ui->DataTable->setRowCount(5);

    char _seg = this->mPara.mps_cSeg;
    _qsl_list.clear();
    _qsl_list = _qs_fileLine.split(_seg, QString::SkipEmptyParts);

    int n_col = _qsl_list.length();

    ui->DataTable->setColumnCount(n_col);

    if(this->mPara.mps_bNoLine1){
        for (int _fe=0; _fe<n_col; _fe++){
            _qsl_head << QString("Col%1").arg(_fe);
            ui->cB0_usrID->addItem(QString("Col%1").arg(_fe));
            ui->cB1_time->addItem(QString("Col%1").arg(_fe));
            ui->cB2_lng->addItem(QString("Col%1").arg(_fe));
            ui->cB3_lat->addItem(QString("Col%1").arg(_fe));

            QTableWidgetItem *item = new QTableWidgetItem(_qsl_list.at(_fe));
            item->setFlags(item->flags() & (~Qt::ItemIsEditable));
            ui->DataTable->setItem(0, _fe, item);
        }
        ui->DataTable->setHorizontalHeaderLabels(_qsl_head);
        for (int _i=1; _i<=4; _i++){
            _qs_fileLine = _qts_inFile.readLine();

            _qsl_list.clear();
            _qsl_list = _qs_fileLine.split(_seg, QString::SkipEmptyParts);
            for (int _fe=0; _fe<n_col; _fe++){
                QTableWidgetItem *item = new QTableWidgetItem(_qsl_list.at(_fe));
                item->setFlags(item->flags() & (~Qt::ItemIsEditable));
                ui->DataTable->setItem(_i, _fe, item);
            }
        }
    }
    else{
        for (int _fe=0; _fe<_qsl_list.length(); _fe++){
            _qsl_head << _qsl_list.at(_fe);
            ui->cB0_usrID->addItem(_qsl_list.at(_fe));
            ui->cB1_time->addItem(_qsl_list.at(_fe));
            ui->cB2_lng->addItem(_qsl_list.at(_fe));
            ui->cB3_lat->addItem(_qsl_list.at(_fe));
        }
        ui->DataTable->setHorizontalHeaderLabels(_qsl_head);
        for (int _i=1; _i<=5; _i++){
            _qs_fileLine = _qts_inFile.readLine();
            _qsl_list.clear();
            _qsl_list = _qs_fileLine.split(_seg, QString::SkipEmptyParts);
            for (int _fe=0; _fe<_qsl_list.length(); _fe++){
                QTableWidgetItem *item = new QTableWidgetItem(_qsl_list.at(_fe));
                item->setFlags(item->flags() & (~Qt::ItemIsEditable));
                ui->DataTable->setItem(_i-1, _fe, item);
            }
        }
    }

    file.close();

    ui->DataTable->setEnabled(true);
    ui->cB0_usrID->setEnabled(true);
    ui->cB1_time->setEnabled(true);
    ui->cB2_lng->setEnabled(true);
    ui->cB3_lat->setEnabled(true);
    ui->cB0_usrID->setCurrentIndex(0);
    ui->cB1_time->setCurrentIndex(1);
    ui->cB2_lng->setCurrentIndex(2);
    ui->cB3_lat->setCurrentIndex(3);

    ui->Para->setEnabled(true);
    ui->check_OK->setEnabled(true);

}

void Setting::previewTime(){
    QDateTime qTime = QDateTime::fromString(QString("19970520230333"), "yyyyMMddHHmmss");
    ui->Time_Lab->setText(qTime.toString(ui->TimeFmt->text()));
}

void Setting::setPara(){
    this->mPara.mps_nCol[0] = ui->cB0_usrID->currentIndex();
    this->mPara.mps_nCol[1] = ui->cB1_time->currentIndex();
    this->mPara.mps_nCol[2] = ui->cB2_lng->currentIndex();
    this->mPara.mps_nCol[3] = ui->cB3_lat->currentIndex();

    this->mPara.mps_nColTxt[0] = ui->cB0_usrID->currentText().toStdString();
    this->mPara.mps_nColTxt[1] = ui->cB1_time->currentText().toStdString();
    this->mPara.mps_nColTxt[2] = ui->cB2_lng->currentText().toStdString();
    this->mPara.mps_nColTxt[3] = ui->cB3_lat->currentText().toStdString();

    this->mPara.mps_qn64GS = ui->GrainSize->text().toLong();
    this->mPara.mps_nPNo = ui->ProNo->text().toInt();
    this->mPara.mps_qsTime_fmt = ui->TimeFmt->text();
    this->mPara.mps_qsTempPath = ui->TempPath->text();
    this->mPara.mps_qn64BS = ui->BufferSize->text().toInt();
    this->mPara.mps_Para[0] = ui->paraH->text().toInt();
    this->mPara.mps_Para[1] = ui->paraW->text().toInt();
    this->mPara.mps_Para[2] = ui->paraD1->text().toInt();
    this->mPara.mps_Para[3] = ui->paraD2->text().toInt();

    QMessageBox _mesg;
    _mesg.information(this,"Great","The setting has been confirmed, you can process now!");
    mw_main->setEn(2, true);
    this->close();
}

void Setting::reset(){
    this->mw_main->reset(0);
}

void Setting::closeWin(){
    this->close();
}

