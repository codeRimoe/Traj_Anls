#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "CDataSet.h"

#include <QMessageBox>
#include <QProgressBar>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mw_set_view(nullptr),
    mw_save(nullptr),
    mw_dsp(nullptr),
	mds_dataSet(nullptr){

	
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());
    //this->setWindowFlags(windowFlags()&~ (Qt::WindowMinMaxButtonsHint));
    this->setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏
    setWindowOpacity(0.9);
    //setAttribute(Qt::WA_TranslucentBackground, true);

    ui->B1_SetPara->setIcon(QIcon(":/img/Set.png"));
    ui->B1_SetPara->setIconSize(QSize(45,45));
    ui->B1_SetPara->setStyleSheet("border-style:flat");
    connect(ui->B1_SetPara, SIGNAL(clicked(bool)), this, SLOT(set_para()));

    ui->B2_Process->setIcon(QIcon(":/img/Process.png"));
    ui->B2_Process->setIconSize(QSize(45,45));
    ui->B2_Process->setStyleSheet("border-style:flat");
    connect(ui->B2_Process, SIGNAL(clicked(bool)), this, SLOT(process()));

    ui->B3_Reload->setIcon(QIcon(":/img/Reload.png"));
    ui->B3_Reload->setIconSize(QSize(45,45));
    ui->B3_Reload->setStyleSheet("border-style:flat");
    connect(ui->B3_Reload, SIGNAL(clicked(bool)), this, SLOT(reload()));

    ui->B4_Save->setIcon(QIcon(":/img/Save.png"));
    ui->B4_Save->setIconSize(QSize(45,45));
    ui->B4_Save->setStyleSheet("border-style:flat");
    connect(ui->B4_Save, SIGNAL(clicked(bool)), this, SLOT(save()));

    ui->B5_Show->setIcon(QIcon(":/img/Show.png"));
    ui->B5_Show->setIconSize(QSize(45,45));
    ui->B5_Show->setStyleSheet("border-style:flat");
    connect(ui->B5_Show, SIGNAL(clicked(bool)), this, SLOT(display()));

    ui->B6_About->setIcon(QIcon(":/img/Analyse.png"));
    ui->B6_About->setIconSize(QSize(45,45));
    ui->B6_About->setStyleSheet("border-style:flat");
    connect(ui->B6_About, SIGNAL(clicked(bool)), this, SLOT(about()));

    ui->B7_Exit->setIcon(QIcon(":/img/Exit.png"));
    ui->B7_Exit->setIconSize(QSize(45,45));
    ui->B7_Exit->setStyleSheet("border-style:flat");
    connect(ui->B7_Exit,SIGNAL(clicked(bool)),this,SLOT(exit()));
}

MainWindow::~MainWindow(){
    delete ui;
	if(mw_set_view)
        delete mw_set_view;
	if(mw_dsp)
    	delete mw_dsp;
	if (mw_save)
		delete mw_save;
	if (mds_dataSet)
		delete mds_dataSet;
}

//实现窗口移动
void MainWindow::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        mb_move = true;
        mqp_startPoint =  event->globalPos();//记录鼠标的世界坐标.
        mqp_windowPoint = this->frameGeometry().topLeft();//记录窗体的世界坐标.
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    if (mb_move)
        if (event->buttons() & Qt::LeftButton){
            //移动中的鼠标位置相对于初始位置的相对位置.
            QPoint _relativePos = event->globalPos() - mqp_startPoint;
            //然后移动窗体即可.
            this->move(mqp_windowPoint + _relativePos );
        }
}
void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton)
        //改变移动状态.
        mb_move = false;
}

void MainWindow::setEn(int nNo, bool bEn){
    switch (nNo) {
    case 1:
        this->ui->B1_SetPara->setEnabled(bEn);
        break;
    case 2:
        this->ui->B2_Process->setEnabled(bEn);
        break;
    case 3:
        this->ui->B3_Reload->setEnabled(bEn);
        break;
    case 4:
        this->ui->B4_Save->setEnabled(bEn);
        break;
    case 5:
        this->ui->B5_Show->setEnabled(bEn);
        break;
    case 6:
        this->ui->B6_About->setEnabled(bEn);
        break;
    case 7:
        this->ui->B7_Exit->setEnabled(bEn);
        break;
    default:
        break;
    }

}

void MainWindow::reset(int exclude=0){
	if (mw_save) {
		delete mw_save;
		mw_save = nullptr;
	}
	if (mw_dsp) {
		delete mw_dsp;
		mw_dsp = nullptr;
	}
	if (mds_dataSet) {
		delete mds_dataSet;
	}
	mds_dataSet = new CDataSet();

	if (exclude) {
		return;
	}

	if (mw_set_view) {
		delete mw_set_view;
		mw_set_view = nullptr;
	}
	setEn(2, false);
	setEn(4, false);
	setEn(5, false);
}

void MainWindow::set_para(){
    if (mw_set_view == nullptr)
        mw_set_view = new Setting(this);
    mw_set_view->show();
}

void MainWindow::process(){
    this->mPara = this->mw_set_view->get_para();
    QString _info = QString("Ready to process!\nThe Parameters will be set as below:\n\n%1\n\nPlease check the Parameters.\nClick Yes to Process;\nClick No to Change the Parameters;\nClick Cancel to cancle."
                            ).arg(this->mPara.show_paras());
    QMessageBox _message(QMessageBox::NoIcon, "Ready", _info, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, NULL);
    _message.setIconPixmap(QPixmap(":/img/start.ico"));
    auto _ans =_message.exec();
    if (_ans == QMessageBox::Yes){

		reset(1);

        qApp->processEvents();
		
        mds_dataSet->load_data(this->mPara.mps_qsFileName,
                                    &this->mPara.mps_cSeg,
                                    this->mPara.mps_nCol,
                                    this->mPara.mps_bNoLine1,
                                    this->mPara.mps_Para,
                                    this->mPara.mps_qn64GS,
                                    this->mPara.mps_qsTime_fmt,
                                    this->ui->progressBar);

        QMessageBox _mesg;
        _mesg.information(this,"Great","The process has been done!");
		setEn(2, false);
		setEn(4, true);
		setEn(5, true);
    }
    else if (_ans == QMessageBox::No){
        this->ui->B1_SetPara->click();
    }
    else{
        return;
    }

}

void MainWindow::reload(){
	
	QMessageBox _msgbox(QMessageBox::NoIcon, "Reload", "...", QMessageBox::Ok, NULL);
	_msgbox.setIconPixmap(QPixmap(":/img/start.ico"));

	_msgbox.setText("Please select a path you want to reload from.\nNote that the path must include 3 files created by this Traj-Analysis application.\nThe files shoule be in the format like:\n*_loc.txt\n*_traj.txt\n*_traj_detail.txt");
	_msgbox.exec();

	QString qsFilename;
	do {
		qsFilename = QFileDialog::getExistingDirectory(this, "Reload", ""); //选择路径
		if (qsFilename == "") {
			_msgbox.setText("No Path Selected.");
			_msgbox.exec();
			return;
		}
		reset(0);
	} while (mds_dataSet->reload(qsFilename, ui->progressBar));

	setEn(4, true);
	setEn(5, true);
}

void MainWindow::save(){
    if (this->mw_save==nullptr)
        mw_save = new Save(this);
    mw_save->show();
}

void MainWindow::save_res(int save_para, QString savename){
    QMessageBox _msgbox(QMessageBox::NoIcon, "Save", "...", QMessageBox::Ok, NULL);
    _msgbox.setIconPixmap(QPixmap(":/img/start.ico"));

    // gml, gj, shp, txt

    QMessageBox _mesg;

    QString qsFilename = QFileDialog::getExistingDirectory(this, "Save", ""); //选择路径
    if(qsFilename == ""){
        _msgbox.setText("No Path Selected.");
        _msgbox.exec();
        return;
    }

    qsFilename += "/Traj_result";
    QDir dir;
    if (dir.exists(qsFilename)){
        int _i = 1;
        while(dir.exists(QString("%1_%2").arg(qsFilename, QString::number(_i))))
            _i++;
        qsFilename = QString("%1_%2").arg(qsFilename, QString::number(_i));
    }

    if (!dir.mkpath(qsFilename)){
        _msgbox.setText("Creat Path Error.");
        _msgbox.exec();
        return;
    }


    if (save_para%10){
        //txt
        _msgbox.setText("Save as Text File (.txt).");
        _msgbox.exec();


        //qsFilename = qsFilename.left(qsFilename.length()-4);

        switch (mds_dataSet->saveTxt(qsFilename, savename)){
            case 1:
                _mesg.warning(this,"Oops", QString("Fail to open file: %1/txt/%2_base.txt!").arg(qsFilename,savename));
                break;
            case 2:
                _mesg.warning(this,"Oops",QString("The files have been saved:\n%1_base.txt\nFail to open file: %1/txt/%2_traj.txt!").arg(
                        qsFilename,savename));
                break;
            case 3:
                _mesg.warning(this,"Oops",QString("The files have been saved:\n%1_base.txt\nFail to open file: %1/txt/%2_traj_detail.txt!").arg(
                        qsFilename,savename));
                break;
            case 4:
                _mesg.warning(this,"Oops",QString("The files have been saved:\n%1_base.txt\nFail to open file: %1/txt/%2_loc.txt!").arg(
                        qsFilename,savename));
                break;
            case 5:
                _mesg.warning(this,"Oops",QString("Creat Txt Path Error!"));
                break;

            default:
                _mesg.information(this,"Great",QString("The files have been saved:\n%1/txt/%2_base.txt\n%1/txt/%2_traj.txt\n%1/txt/%2_traj_detail.txt\n%1/txt/%2_loc.txt").arg(
                            qsFilename,savename));
                break;
        }
    }

    save_para /= 10;
    if (save_para){
        _msgbox.setText("Save as Geo File.");
        _msgbox.exec();
    }


    if (save_para%10){
        saveGeoTip(qsFilename, savename, "shp");
    }

    save_para /= 10;
    if (save_para%10){
        saveGeoTip(qsFilename, savename, "geojson");
    }

    save_para /= 10;
    if (save_para%10){
        saveGeoTip(qsFilename, savename, "gml");
    }

}


inline void MainWindow::saveGeoTip(QString qsFilePath, QString savename, QString fT){
    int res = mds_dataSet->saveGeo(qsFilePath, savename, fT);
    QMessageBox _mesgBox;

    QString mesg="";
    QString _tstr = qsFilePath + '/' + fT;

    if(res == 0){
        _mesgBox.information(this,"Great",QString("The files have been saved:\n%1/%2_base.%3\n%1/%2_traj.%3").arg(_tstr, savename, fT));
        return;
    }

    else if(res<20){
        mesg = "File Error: \n";
        if(res == 10)
            mesg += QString("Type Error!");
        else if (res == 11)
            mesg += QString("Fail to create path %1!").arg(_tstr);
        else if (res == 12)
            mesg += QString("Fail to activate %1 Driver!").arg(fT);
    }
    else{
        int type = res/10, type2 =type%10;

        if ((type == 2) || (type == 3))
            mesg = QString("Error when saving Base: %1/%2_base.%3\n").arg(_tstr, savename, fT);
        else if ((type == 4) || (type == 5))
            mesg = QString("The files have been saved:\n%1/%2_base.%3\nError when saving Base: %1/%2_traj.%3\n").arg(_tstr, savename, fT);
        else
            mesg = "Unknow Error.";

        if((type == 2) || (type == 4)){
            switch (type2) {
            case 1:
                mesg += QString("DataSet Creation Error!");
                break;
            case 2:
                mesg += QString("Layer Creation Failed!");
                break;
            default:
                break;
            }
        }
        else if ((type == 3) || (type == 5)){
            mesg = "Create Feature Error.";
            switch (type2) {
            case 1:
                mesg += QString("Not enough data to deserialize.");
                break;
            case 2:
                mesg += QString("Not enough memory.");
                break;
            case 3:
                mesg += QString("Unsupported geometry type.");
                break;
            case 4:
                mesg += QString("Unsupported operation.");
                break;
            case 5:
                mesg += QString("Corrupt data.");
                break;
            case 6:
                mesg += QString("Failure.");
                break;
            case 7:
                mesg += QString("Unsupported SRS.");
                break;
            case 8:
                mesg += QString("Invalid handle.");
                break;
            case 9:
                mesg += QString("Non existing feature.");
                break;
            default:
                break;
            }
        }
    }
    _mesgBox.warning(this,"Oops",mesg);
}

void MainWindow::display(){
    if (mw_dsp == nullptr)
        mw_dsp = new Display(mds_dataSet);
    mw_dsp->show();
    mw_dsp->see_all();
//	mw_dsp->loadUserList();
}

void MainWindow::about(){
	QString qs_about = QString("This is a software application for <font color='red'>cellular signaling data analysis and visualization</font> by QT, OpenGL and GDAL.\n");
	qs_about += QString("The software application is provided by Department of Grographic Information Science, <a href='http://gp.sysu.edu.cn'>School of Geography and Planning</a>, <a href='http://sysu.edu.cn'>Sun Yat-sen University</a>. All right reserved.\n");
	qs_about += QString("\n</br>\n");
	qs_about += QString("The application is mainly created by Yue/Haowen Luo.\n");
	qs_about += QString("For more information, please refer to his personal website <a href='http://rimoe.xyz'>Rimoe - 柠萌</a>.\n");
	qs_about += QString("Or you can <a href='mailto://yue.rimoe@gmail.com'>send him an email</a>.\n");
	qs_about += QString("\n</br>\n");
	qs_about += QString("To learn how to use this software or get the source files, please refer to <a href='https://github.com/codeRimoe/Traj_Anls'>the github page</a>.\n");

	QMessageBox _message(QMessageBox::NoIcon, "About", "", QMessageBox::Ok, NULL);
	_message.setIconPixmap(QPixmap(":/img/start.ico"));
	_message.setText(qs_about);
	_message.setDefaultButton(QMessageBox::Ok);
	_message.exec();

}

void MainWindow::exit()
{
    QMessageBox _message(QMessageBox::NoIcon, "Caution", "Are you sure to exit?", QMessageBox::Yes | QMessageBox::Cancel, NULL);
    _message.setIconPixmap(QPixmap(":/img/start.ico"));
    if (_message.exec() == QMessageBox::Yes)
        qApp->quit();
	_message.setDefaultButton(QMessageBox::Cancel);
}
