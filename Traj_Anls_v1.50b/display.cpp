#include "display.h"
#include "ui_display.h"
#include <QDateTime>
#include <QMessageBox>
#include <QFileDialog>
#include <QList>
#include <vector>
using std::vector;
Display::Display(CDataSet *cds ,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Display),
    mp_ds(cds),
	mpt_currentTraj{ nullptr },
	mpb_selectedBase{ nullptr }
{
    ui->setupUi(this);
    ui->openGLWidget->set_ds(mp_ds);
	loadUserList(-1);
    ui->openGLWidget->show();
	
    connect(ui->Btn_SeeAll, SIGNAL(clicked(bool)), this, SLOT(see_all()));
	connect(ui->Btn_Zoom, SIGNAL(clicked(bool)), this, SLOT(see_traj()));
	connect(ui->Btn_Clear, SIGNAL(clicked(bool)), this, SLOT(clear_traj()));
	connect(ui->Btn_Exp, SIGNAL(clicked(bool)), this, SLOT(export_traj()));
	connect(ui->list_user, SIGNAL(itemSelectionChanged()), this, SLOT(loadSelectedUser()));
	connect(ui->cb_DetailUser, SIGNAL(currentTextChanged(const QString &)), this, SLOT(loadSelectedTraj()));
	connect(ui->list_StayP, SIGNAL(currentRowChanged(int)), this, SLOT(showDetail()));
	connect(ui->list_LocP, SIGNAL(currentRowChanged(int)), this, SLOT(showDetail()));
	connect(ui->tabPoint, SIGNAL(currentChanged(int)), this, SLOT(showDetail()));
}

Display::~Display()
{
    delete ui;
}


void Display::mousePressEvent(QMouseEvent *event){
    if (ui->openGLWidget->geometry().contains((mapFromGlobal(QCursor::pos())))){
        if(event->button() == Qt::RightButton){
            mb_move = true;
            mqp_startPoint =  event->globalPos(); //world coor of the mouse.
        }
    }
}

void Display::mouseMoveEvent(QMouseEvent *event){
    if (mb_move)
        if (event->buttons() & Qt::RightButton){
            // the location of moving mouse (relativa to init-location）
            QPoint _relativePos = event->globalPos() - mqp_startPoint;
            // move the window

            ui->openGLWidget->add_center(double(_relativePos.x()),
                                         double(_relativePos.y()));
            mqp_startPoint =  event->globalPos();
            this->update();
        }
}
void Display::mouseReleaseEvent(QMouseEvent *event){
    if (event->button() == Qt::RightButton)
        // change the move state
        mb_move = false;
}

void Display::wheelEvent(QWheelEvent *event){
    if (ui->openGLWidget->geometry().contains((mapFromGlobal(QCursor::pos())))){
            setCursor(Qt::BusyCursor);
            //if ( QApplication::keyboardModifiers () == Qt::ControlModifier){
            //    if (event->angleDelta().y() > 0)
            //        ui->openGLWidget->mul_vDg(1.05);
            //    else
            //        ui->openGLWidget->mul_vDg(0.95);
            //}
            //else if ( QApplication::keyboardModifiers () == Qt::ShiftModifier){
            //    if (event->angleDelta().y() > 0)
            //        ui->openGLWidget->add_split(5);
            //    else
            //        ui->openGLWidget->add_split(-5);
            //}
            //else{
            if (event->angleDelta().y() > 0)
                ui->openGLWidget->mul_scale(1.05);
            else
                ui->openGLWidget->mul_scale(0.95);
            //}

            setCursor(Qt::ArrowCursor);
    }
}

void Display::see_all(){
    this->ui->openGLWidget->see_all();
    this->ui->openGLWidget->update();
}


void Display::see_traj() {
	this->ui->openGLWidget->see_traj();
	this->ui->openGLWidget->update();
}

void Display::clear_traj() {
	ui->cb_DetailUser->clear();
	mpt_currentTraj = nullptr;
	mpb_selectedBase = nullptr;
	loadUserList(-1);
}

void Display::show_selected_traj() {
	ui->openGLWidget->set_currentTraj(mpt_currentTraj);
	ui->openGLWidget->set_selectedBase(mpb_selectedBase);
	ui->openGLWidget->set_selectedTrajs(get_selectedTrajs());
	this->ui->openGLWidget->update();
}

vector<CTraj*> Display::get_selectedTrajs() {
	vector<CTraj*> opt;
	opt.clear();
	QList<QListWidgetItem*> pql = ui->list_user->selectedItems();
	for (QList<QListWidgetItem*>::iterator iter = pql.begin(); iter != pql.end(); iter++) {
		opt.emplace_back(mm_selected_user[(*iter)->text()]);
	}
	return opt;
}

CTraj* Display::get_currentTraj() {
	return mpt_currentTraj;
}

CBase* Display::get_selectedBase() {
	return mpb_selectedBase;
}

void Display::loadUserList(int n_max=-1) {
	ui->list_user->clear();
	vector<CTraj> *mp_tjs = mp_ds->getCTrajs();
	int n_len = mp_tjs->size();
	if ((n_max != -1) && (n_len > n_max))
		n_len = n_max;
	for (vector<CTraj>::iterator iter = mp_tjs->begin(); iter != mp_tjs->end(); iter++, n_len--) {
		QString qs_uid = QString::fromStdString(iter->getUsrID());
		ui->list_user->addItem(qs_uid);
		mm_selected_user[qs_uid] = &(*iter);
		if (n_len <= 0)
			break;
	}
}

void Display::loadSelectedUser() {
	QList<QListWidgetItem*> pql = ui->list_user->selectedItems();
	ui->cb_DetailUser->clear();
	ui->ls_detail->clear();
	for (QList<QListWidgetItem*>::iterator iter = pql.begin(); iter != pql.end(); iter++) {
		ui->cb_DetailUser->addItem((*iter)->text());
	}
	show_selected_traj();
}

void Display::loadSelectedTraj() {
	disconnect(ui->list_LocP, 0, 0, 0);;
	disconnect(ui->list_StayP, 0, 0, 0);;

	ui->list_StayP->clear();
	ui->list_LocP->clear();

	auto finder = mm_selected_user.find(ui->cb_DetailUser->currentText());
	if (finder == mm_selected_user.end()) {
		mpt_currentTraj = nullptr;
	    return;
    }
	mpt_currentTraj = finder->second;

	vector<CStay>* p_stay_points = mpt_currentTraj->getStayPoints();
	unordered_map<int, CLocation>* p_locations = mpt_currentTraj->getLocation();
	
	for (vector<CStay>::iterator iter = p_stay_points->begin(); iter != p_stay_points->end(); iter++) {
		CBase* pb = iter->get_Base();
		CLocation* pl = mpt_currentTraj->find_Location(pb);
		ui->list_StayP->addItem(QString("%1 (%2)").arg(QString::fromStdString(pb->get_Pid()), QString::fromStdString(pl->get_SPType_s())));
	}

	mpl_pre.clear();
	for (unordered_map<int, CLocation>::iterator iter = p_locations->begin(); iter != p_locations->end(); iter++) {
		CLocation loc = iter->second;
		CBase* pb = loc.get_Base();
		mpl_pre.emplace_back(loc);
		ui->list_LocP->addItem(QString("%1 (%2)").arg(QString::fromStdString(pb->get_Pid()), QString::fromStdString(loc.get_SPType_s())));
	}
	
	ui->list_LocP->setCurrentRow(0);
	ui->list_StayP->setCurrentRow(0);
	showDetail();
	connect(ui->list_StayP, SIGNAL(currentRowChanged(int)), this, SLOT(showDetail()));
	connect(ui->list_LocP, SIGNAL(currentRowChanged(int)), this, SLOT(showDetail()));
	show_selected_traj();
}

void Display::showDetail() {
	CLocation* pl = nullptr;
	vector<CStay>* p_stay_points = mpt_currentTraj->getStayPoints();
	vector<QString> qs_times;
	qs_times.clear();

	if ((ui->list_StayP->count() <= 0) || (ui->list_LocP->count() <= 0) || (mpt_currentTraj == nullptr)) {
		mpb_selectedBase = nullptr;
		return;
	}
	if (ui->tabPoint->currentIndex() == 0) {
		CStay pst = p_stay_points->at(ui->list_StayP->currentRow());
		mpb_selectedBase = pst.get_Base();
		pl = mpt_currentTraj->find_Location(mpb_selectedBase);

		QString qs_STime = QDateTime::fromTime_t(pst.get_StartT()).toString("yyyy-MM-dd hh:mm:ss");
		QString qs_ETime = QDateTime::fromTime_t(pst.get_EndT()).toString("yyyy-MM-dd hh:mm:ss");
		qs_times.emplace_back(QString("From {\t%1").arg(qs_STime));
		qs_times.emplace_back(QString("To\t%1}").arg(qs_ETime));
	}
	else {
		pl = &mpl_pre[ui->list_LocP->currentRow()];
		mpb_selectedBase = pl->get_Base();

		for (vector<CStay>::iterator iter = p_stay_points->begin(); iter != p_stay_points->end(); iter++) {
			if (iter->get_Base() != mpb_selectedBase)
				continue;
			QString qs_STime = QDateTime::fromTime_t(iter->get_StartT()).toString("yyyy-MM-dd hh:mm:ss");
			QString qs_ETime = QDateTime::fromTime_t(iter->get_EndT()).toString("yyyy-MM-dd hh:mm:ss");
			qs_times.emplace_back(QString("From {\t%1").arg(qs_STime));
			qs_times.emplace_back(QString("To\t%1}").arg(qs_ETime));
		}
	}
	
	ui->ls_detail->clear();
	ui->ls_detail->addItem(QString("User ID: %1").arg(QString::fromStdString(mpt_currentTraj->getUsrID())));
	ui->ls_detail->addItem(QString("Location Type: %1").arg(QString::fromStdString(pl->get_SPType_s())));
	ui->ls_detail->addItem(QString("Day/Night Time(min): %1 / %2").arg(QString::number(pl->getDay()), QString::number(pl->getNight())));
	ui->ls_detail->addItem("");
	ui->ls_detail->addItem(QString("BaseStation ID: %1").arg(QString::fromStdString(mpb_selectedBase->get_Pid())));
	ui->ls_detail->addItem(QString("Lng/Lat: %1 / %2").arg(QString::number(mpb_selectedBase->get_Lng()), QString::number(mpb_selectedBase->get_Lat())));
	ui->ls_detail->addItem("");
	for (vector<QString>::iterator iter = qs_times.begin(); iter != qs_times.end(); iter++) {
		ui->ls_detail->addItem(*iter);
	}

	show_selected_traj();
}

void Display::export_traj() {
	if (mpt_currentTraj == nullptr) {
		QMessageBox _message(QMessageBox::Information, "Note", "Please selected a traj first.", QMessageBox::Ok, NULL);
		_message.exec();
		return;
	}
	QString qs_about = QString("Save all Traj or selected Traj ? \nClick Save_all to save all traj.\nClick Save to save selected traj.");
	QMessageBox _message(QMessageBox::NoIcon, "Qusetion", qs_about, QMessageBox::SaveAll | QMessageBox::Save | QMessageBox::Cancel, NULL);
	_message.setIconPixmap(QPixmap(":/img/start.ico"));
	_message.setDefaultButton(QMessageBox::Save);
	auto r = _message.exec();

	if (r == QMessageBox::Cancel)
		return;

	QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", "GeoJson (*.geojson *.json);;GML (*.gml);;ESRI ShapeFile (*.shp);;");

	if (fileName.isNull())
		return;
	QList<QString> ql_f = fileName.split('.');
	string qsT = ql_f[ql_f.size()-1].toLower().toStdString();

	if (qsT == "shp")
		qsT = "ESRI Shapefile";
	else if (qsT == "gml")
		qsT = "GML";
	else if ((qsT == "geojson") || (qsT == "json"))
		qsT = "GeoJSON";
	else {
		fileName += QString(".geojson");
		qsT = "GeoJSON";
	}

	int rt = 0;
	if (r == QMessageBox::SaveAll)
		rt = mp_ds->saveTraj(get_selectedTrajs(), fileName, qsT);
	else if (r == QMessageBox::Save)
		rt = mp_ds->saveTraj(mpt_currentTraj, fileName, qsT);

	if (rt) {
		QMessageBox _message2(QMessageBox::Warning, "Oops!", "Something went wrong when saving.", QMessageBox::Ok, NULL);
		_message2.exec();
	}
	else{
		QMessageBox _message2(QMessageBox::Information, "Great!", "Saved.", QMessageBox::Ok, NULL);
		_message2.exec();
	}
	
}

