#include <iostream>
#include <fstream>
#include "CDataSet.h"
#include <iomanip>
// #include <io.h>
#include <string>
#include <unordered_map>
#include <map>
#include <ctime>

#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QList>

#include "ogrsf_frmts.h"
#include "gdal.h"
#include "gdal_priv.h"

using std::cin; using std::cout; using std::endl;

CDataSet::CDataSet(){
}


CDataSet::~CDataSet(){
}

unordered_map<CoorXY, CBase, hashFunc>* CDataSet::getBS(){
    return &mhmapBaseS;
}

vector<CTraj>* CDataSet::getCTrajs() {
	return &mvTrajs;
}


void CDataSet::load_data(QString sFilePath, char* pcSep, int* nCol, bool b_l1_data, int* nPara, qint64 qn64_GS, QString _qsTfmt, QProgressBar* pProgressBar) {
	StaticPara sp_data;

    time_t t_aclock = clock()/CLOCKS_PER_SEC;
    mqsTfmt = _qsTfmt;

    QFile qf_file(sFilePath);
    if(!qf_file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return;
    }

    qint64 qn64_ofs = 0, qn64_fSIZE = qf_file.size()/sizeof(char), _i=0;
    qn64_GS /= sizeof(uchar);

    double _tmp = 0;
    int _tmp_time = 0;
    bool b_data_now = b_l1_data;

    while(qn64_ofs < qn64_fSIZE){
        if (qn64_fSIZE - qn64_ofs < qn64_GS)
            qn64_GS = qn64_fSIZE - qn64_ofs;
        uchar *puc_fpr = qf_file.map(qn64_ofs, qn64_GS);

        for (int _j=0; _j<qn64_GS; _j++,_i++){
            if (_i % 10000 == 0){
                _tmp_time = clock()/CLOCKS_PER_SEC - t_aclock;
                _tmp = double(_i) / qn64_fSIZE;

                pProgressBar->setValue(int(100 * _tmp));
                pProgressBar->setFormat(QString("Use time/Total Time:%1s/%2s | Progress:%3%").arg(
                                            QString::number(_tmp_time),
                                            QString::number(_tmp_time/_tmp, 'f', 2),
                                            QString::number(100 * _tmp, 'f', 2)));
            }
            if (b_data_now)
                process_input((char*)(puc_fpr + _j), pcSep, nCol, nPara, &sp_data);
            else
                if (*(puc_fpr + _j)=='\n')
                    b_data_now=true;
        }
        qf_file.unmap(puc_fpr);
        qn64_ofs += qn64_GS;
    }
    pProgressBar->setValue(100);
    pProgressBar->setFormat(QString("Use time:%1s | Progress:Done").arg(QString::number(_tmp_time)));
}

inline void CDataSet::process_input(char* pcInChar, char* pcSep, int* nCol, int* nPara, StaticPara* sp_data) {

    //static string sTemp(""), sUserID(""), sEndTime("");
    //static int nSBaseID{ 0 };
    //static vector<string> vsTemp;

    //static CTraj *ptTempTraj = nullptr;
    //static CStay *psTempStay = nullptr;
    //static CBase *pbTempBase = nullptr;

    if (*pcInChar == *pcSep) {
		sp_data->vsTemp.emplace_back(sp_data->sTemp);
		sp_data->sTemp.clear();
        return;
    }

    // finish a row
    if (*pcInChar == '\n') {
		sp_data->vsTemp.emplace_back(sp_data->sTemp);
		sp_data->sTemp.clear();

        // different user
        if (sp_data->sUserID != sp_data->vsTemp[nCol[0]]) {

            if (sp_data->psTempStay != nullptr) {
                time_t _ttTend = readTime(sp_data->sEndTime);
				sp_data->psTempStay->set_EndT(_ttTend);   //update old stay endtime
                int nStayTime[2]{ 0 };
				sp_data->psTempStay->get_StayT(nStayTime,nPara);
				sp_data->ptTempTraj->find_Location(sp_data->psTempStay->get_Base())->addTime(nStayTime);  //update old CLoc's stay time
				sp_data->ptTempTraj->label(nPara);
				sp_data->ptTempTraj->classify();
            }

            time_t _ttT = readTime(sp_data->vsTemp[nCol[1]]);

            // new a traj
			sp_data->sUserID = sp_data->vsTemp[nCol[0]];
			mvTrajs.emplace_back(sp_data->sUserID);
			sp_data->ptTempTraj = &mvTrajs.back();

			sp_data->pbTempBase = push_BaseStation(stod(sp_data->vsTemp[nCol[2]]), stod(sp_data->vsTemp[nCol[3]]), std::to_string(sp_data->nSBaseID++));
			sp_data->psTempStay = sp_data->ptTempTraj->pushStayPoint(sp_data->pbTempBase, _ttT);

        }

        // the same user
        else {
            CBase *_pbTempBase = push_BaseStation(stod(sp_data->vsTemp[nCol[2]]), stod(sp_data->vsTemp[nCol[3]]), std::to_string(sp_data->nSBaseID));
            // different base
            if (_pbTempBase != sp_data->pbTempBase) {
                time_t _ttTend = readTime(sp_data->sEndTime);
                time_t _ttT = readTime(sp_data->vsTemp[nCol[1]]);
                _ttT += (_ttTend-_ttT) / 2;
				sp_data->psTempStay->set_EndT(_ttT);   //update old stay endtime
                int nStayTime[2]{ 0 };
				sp_data->sEndTime = sp_data->vsTemp[nCol[1]];
				sp_data->psTempStay->get_StayT(nStayTime, nPara);
				sp_data->ptTempTraj->find_Location(sp_data->psTempStay->get_Base())->addTime(nStayTime);  //update CLoc's stay time

				sp_data->nSBaseID++;
				sp_data->pbTempBase = _pbTempBase;
				sp_data->psTempStay = sp_data->ptTempTraj->pushStayPoint(sp_data->pbTempBase, _ttT);
            }
        }
		sp_data->sEndTime = sp_data->vsTemp[nCol[1]];
        //ptTempTraj->find_Location(pbTempBase)->dayPlus();
        //ptTempTraj->find_Location(pbTempBase)->nightPlus();
		sp_data->vsTemp.clear();
        return;
    }
    if ((*pcInChar!='\r')&&((*pcInChar!='\0')))
		sp_data->sTemp += *pcInChar;
    return;

}

int CDataSet::reload(QString path, QProgressBar* pProgressBar) {
	pProgressBar->setValue(0);
	pProgressBar->setFormat(QString("Opening Reload File... | Progress:0%"));
	QDir dir{ path };
	QList<QString> ql_fn = dir.entryList();
	QString qs[3] = {};  
	for (int _i = 0; _i < ql_fn.size(); _i++) {
		QString _qs = ql_fn[_i];
		if (_qs.right(16) == "_traj_detail.txt")
			qs[0] = _qs;
		else if (_qs.right(8) == "_loc.txt")
			qs[1] = _qs;
		else if (_qs.right(9) == "_traj.txt")
			qs[2] = _qs;
	}

	if ((qs[0] == "") || (qs[1] == "")) {
		pProgressBar->setValue(100);
		pProgressBar->setFormat(QString("Reload Fail. | Progress:100%"));
		return 1;
	}

	QFile traj_file(path + "/" + qs[0]);
	if (!traj_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		pProgressBar->setValue(100);
		pProgressBar->setFormat(QString("Reload Fail. | Progress:100%"));
		return 1;
	}

	QFile loc_file(path + "/" + qs[1]);
	if (!loc_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		pProgressBar->setValue(100);
		pProgressBar->setFormat(QString("Reload Fail. | Progress:100%"));
		return 1;
	}

	// TODO:qs[2] for traj file

	pProgressBar->setFormat(QString("Reloading Traj_detail... | Progress:45%"));
	pProgressBar->setValue(45);
	// ====================================================
	// read Traj_detail file
	mqsTfmt = "yyyy-MM-dd hh:mm:ss";
	traj_file.readLine();
	QString tmp_User_Id("");
	unordered_map<string, CBase*> hm_locs;
	while (!traj_file.atEnd()) {
		QString qs_line = QString(traj_file.readLine().trimmed());
		if (qs_line == "")
			continue;
		QList<QString> line = qs_line.split('\t');
		
		if (line[0] != tmp_User_Id) {
			tmp_User_Id = line[0];
			mvTrajs.emplace_back(tmp_User_Id.toStdString());
		}
		time_t _ttT = readTime(line[2].toStdString()), _ttTend = readTime(line[3].toStdString());

		CBase* pbTempBase = push_BaseStation(line[4].toDouble(), line[5].toDouble(), line[1].toStdString());
		hm_locs[line[1].toStdString()] = pbTempBase;
		mvTrajs.back().pushStayPoint(pbTempBase, _ttT)->set_EndT(_ttTend);
	}
	traj_file.close();

	pProgressBar->setFormat(QString("Reloading Location... | Progress:65%"));
	pProgressBar->setValue(65);
	// ====================================================
	// read Loc file
	loc_file.readLine();
	vector<CTraj>::iterator iter = mvTrajs.begin();
	while (!loc_file.atEnd()) {
		QString qs_line = QString(loc_file.readLine().trimmed());
		if (qs_line == "")
			continue;
		QList<QString> line = qs_line.split('\t');
		if (line[0] != QString::fromStdString(iter->getUsrID())) {
			++iter;
		}
		iter->find_Location(hm_locs[line[1].toStdString()])->load_loc(line[2].toStdString(), line[3].toInt(), line[4].toInt());
	}
	loc_file.close();

	pProgressBar->setFormat(QString("Reloading Traj... | Progress:80%"));
	pProgressBar->setValue(85);
	// ====================================================
	// read Traj file

	pProgressBar->setValue(100);
	pProgressBar->setFormat(QString("Reloaded | Progress:Done"));

	return 0;
}

int CDataSet::saveTxt(QString qsFilePath, QString savename){
    QDir dir;
    qsFilePath += "/txt";

    if (!dir.mkpath(qsFilePath)){
        return 5;
    }
    qsFilePath += '/' + savename;
    string sFilePath= qsFilePath.toStdString();
    // ====================================================
    // output Base file
    std::ofstream foutBase(sFilePath + "_base.txt");
    if (!foutBase) {
        return 1;
    }

    foutBase << "BasePID\tLng\tLat" << endl;
    for (auto _it = mhmapBaseS.begin(); _it != mhmapBaseS.end(); ++_it) {
        foutBase << _it->second.outString() << endl;
    }

    foutBase.close();

    // ====================================================
    // output Traj and Stay
    std::ofstream foutTraj(sFilePath + "_traj.txt");
    if (!foutTraj) {
        return 2;
    }

    std::ofstream foutTrajd(sFilePath + "_traj_detail.txt");
    if (!foutTrajd) {
        return 3;
    }

    std::ofstream foutStay(sFilePath + "_loc.txt");
    if (!foutStay) {
        return 4;
    }

    foutTrajd << "User_ID\tBasePID\tStartTime\tEndTime\tLng\tLat" << endl;
    foutStay << "User_ID\tBasePID\tType\tDayTime(min)\tNightTime(min)" << endl;
    for (auto _it = mvTrajs.begin(); _it != mvTrajs.end(); ++_it) {
        vector<CStay> *_pcsSPs = _it->getStayPoints();
        unordered_map<int, CLocation> *_pmmapLoc = _it->getLocation();
        foutTraj << _it->outString() << endl;
        for (auto _jt = _pcsSPs->begin(); _jt != _pcsSPs->end(); ++_jt) {
            CBase* _tpBase = _jt->get_Base();
            foutTrajd << _it->getUsrID() << "\t" <<
                         _tpBase->get_Pid() << "\t" <<
                         saveTime(_jt->get_StartT()) << "\t" <<
                         saveTime(_jt->get_EndT()) << "\t" <<
                         std::to_string(_tpBase->get_Lng()) << "\t" <<
                         std::to_string(_tpBase->get_Lat()) << endl;
        }
        for (auto _jt = _pmmapLoc->begin(); _jt != _pmmapLoc->end(); ++_jt) {
            foutStay << _it->getUsrID() << "\t" << _jt->second.outString() << endl;
        }
    }

    foutTraj.close();
    foutTrajd.close();
    foutStay.close();

    return 0;
}


int CDataSet::saveGeo(QString qsFilePath, QString savename, QString fT){
    QDir dir;
    QString allPath, qsType = "shp";

    string pc_driver;

    //使属性表字段支持中文
    CPLSetConfigOption("SHAPE_ENCODING","");
    GDALAllRegister();//注册所有的驱动

    if (fT == "shp")
        pc_driver = "ESRI Shapefile";
    else if (fT == "geojson")
        pc_driver = "GeoJSON";
    else if (fT == "gml")
        pc_driver = "GML";
    else
        return 10;        //Type Error

    if (!dir.mkpath(qsFilePath + "/" + fT))
        return 11;        //Create path error

    //调用对Shape文件读写的Driver
    GDALDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pc_driver.c_str());
    if (poDriver == NULL)
        return 12;        //Driver Error

    // ====================================================
    // output Base file

    //创建数据源
    allPath = qsFilePath + '/' + fT + '/' + savename + "_base." + fT;
    GDALDataset *poDs = poDriver->Create(allPath.toLatin1(), 0, 0, 0, GDT_Unknown, NULL);
    if (poDs == NULL)
        return 21;        //DataSource Creation Error

    //创建图层Layer
    //参数说明：新图层名称，坐标系，图层的几何类型，创建选项，与驱动有关
    OGRLayer *poLayer = poDs->CreateLayer("BaseStation", NULL, wkbPoint, NULL);
    if (poLayer == NULL){
        GDALClose(poDs);
        return 22;        //Layer Creation Failed
    }

    //下面创建属性表，我们在属性表中创建两列数据即可
    OGRFieldDefn oFieldId("BasePID", OFTString);
    oFieldId.SetWidth(20);
    poLayer->CreateField(&oFieldId);

    OGRFieldDefn oFieldX("Lng", OFTString);
    oFieldX.SetWidth(30);
    poLayer->CreateField(&oFieldX);

    OGRFieldDefn oFieldY("Lat", OFTString);
    oFieldY.SetWidth(30);
    poLayer->CreateField(&oFieldY);

    for (auto _it = mhmapBaseS.begin(); _it != mhmapBaseS.end(); ++_it) {

        //创建一个feature
        OGRFeature *poFeature;
        poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());  //GetLayerDefn()获取当前图层的属性表结构

        double _d_lng = _it->second.get_Lng();
        double _d_lat = _it->second.get_Lat();

        //给属性表中我们刚创建的列赋值
        poFeature->SetField("BasePID", _it->second.get_Pid().c_str());
        poFeature->SetField("Lng", _d_lng);
        poFeature->SetField("Lat", _d_lat);

        //创建一个OGRPoint对象
        OGRPoint point;
        point.setX(_d_lng);
        point.setY(_d_lat);
        //point.setZ(0);

        poFeature->SetGeometry(&point);

        if(poLayer->CreateFeature(poFeature) != OGRERR_NONE ){
            return 30 + poLayer->CreateFeature(poFeature);

            //OGRERR_NONE                       0 Success.
            //OGRERR_NOT_ENOUGH_DATA            1 Not enough data to deserialize.
            //OGRERR_NOT_ENOUGH_MEMORY          2 Not enough memory.
            //OGRERR_UNSUPPORTED_GEOMETRY_TYPE  3 Unsupported geometry type.
            //OGRERR_UNSUPPORTED_OPERATION      4 Unsupported operation.
            //OGRERR_CORRUPT_DATA               5 Corrupt data.
            //OGRERR_FAILURE                    6 Failure.
            //OGRERR_UNSUPPORTED_SRS            7 Unsupported SRS.
            //OGRERR_INVALID_HANDLE             8 Invalid handle.
            //OGRERR_NON_EXISTING_FEATURE       9 Non existing feature

        }
        OGRFeature::DestroyFeature(poFeature);
    }
    GDALClose(poDs);


    // ====================================================
    // output Traj

    //创建数据源
    allPath = qsFilePath + '/' + fT + '/' + savename + "_traj." + fT;
    poDs = poDriver->Create(allPath.toLatin1(), 0, 0, 0, GDT_Unknown, NULL);
    if (poDs == NULL)
        return 41;        //DataSource Creation Error

    //创建图层Layer
    //参数说明：新图层名称，坐标系，图层的几何类型，创建选项，与驱动有关
    poLayer = poDs->CreateLayer("Traj", NULL, wkbLineString, NULL);
    if (poLayer == NULL){
        GDALClose(poDs);
        return 42;        //Layer Creation Failed
    }

    //下面创建属性表，我们在属性表中创建两列数据即可
    OGRFieldDefn oFuId("UserID", OFTString);
    oFuId.SetWidth(50);
    poLayer->CreateField(&oFuId);

    OGRFieldDefn oFT("Traj", OFTString);
    oFT.SetWidth(100);
    poLayer->CreateField(&oFT);


    for (auto _it = mvTrajs.begin(); _it != mvTrajs.end(); ++_it) {

        //创建一个feature
        OGRFeature *poFeature;
        poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());  //GetLayerDefn()获取当前图层的属性表结构

        //创建一个OGRPoint对象
        OGRLineString line;

        string qs_types = "";

        vector<CStay> *_pcsSPs = _it->getStayPoints();

        for (int _j = 0; _j < _pcsSPs->size(); ++_j) {
            CBase *_pB = _pcsSPs->at(_j).get_Base();
            qs_types += _it->find_Location(_pB)->get_SPType_s() + " ";
            line.setPoint(_j, _pB->get_Lng(), _pB->get_Lat());
        }

        poFeature->SetField("UserID", _it->getUsrID().c_str());
        poFeature->SetField("Traj", qs_types.c_str());

        poFeature->SetGeometry(&line);

        if(poLayer->CreateFeature(poFeature) != OGRERR_NONE )
            return 50 + poLayer->CreateFeature(poFeature);
        OGRFeature::DestroyFeature(poFeature);

    }

    GDALClose(poDs);

    return 0;

}

int CDataSet::saveTraj(CTraj* saveT, QString saveP, string pc_driver) {

	//使属性表字段支持中文
	CPLSetConfigOption("SHAPE_ENCODING", "");
	GDALAllRegister();//注册所有的驱动

					  //调用对Shape文件读写的Driver
	GDALDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pc_driver.c_str());
	if (poDriver == NULL)
		return 12;        //Driver Error

						  // ====================================================
						  // output Traj

						  //创建数据源
	GDALDataset *poDs = poDriver->Create(saveP.toLatin1(), 0, 0, 0, GDT_Unknown, NULL);
	if (poDs == NULL)
		return 41;        //DataSource Creation Error

						  //创建图层Layer
						  //参数说明：新图层名称，坐标系，图层的几何类型，创建选项，与驱动有关
	OGRLayer *poLayer = poDs->CreateLayer("Traj", NULL, wkbLineString, NULL);
	if (poLayer == NULL) {
		GDALClose(poDs);
		return 42;        //Layer Creation Failed
	}

	//下面创建属性表，我们在属性表中创建两列数据即可
	OGRFieldDefn oFuId("UserID", OFTString);
	oFuId.SetWidth(50);
	poLayer->CreateField(&oFuId);

	OGRFieldDefn oFT("Traj", OFTString);
	oFT.SetWidth(100);
	poLayer->CreateField(&oFT);


	//创建一个feature
	OGRFeature *poFeature;
	poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());  //GetLayerDefn()获取当前图层的属性表结构

																	 //创建一个OGRPoint对象
	OGRLineString line;

	string qs_types = "";

	vector<CStay> *_pcsSPs = saveT->getStayPoints();

	for (int _j = 0; _j < _pcsSPs->size(); ++_j) {
		CBase *_pB = _pcsSPs->at(_j).get_Base();
		qs_types += saveT->find_Location(_pB)->get_SPType_s() + " ";
		line.setPoint(_j, _pB->get_Lng(), _pB->get_Lat());
	}

	poFeature->SetField("UserID", saveT->getUsrID().c_str());
	poFeature->SetField("Traj", qs_types.c_str());

	poFeature->SetGeometry(&line);

	if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
		return 50 + poLayer->CreateFeature(poFeature);
	OGRFeature::DestroyFeature(poFeature);

	GDALClose(poDs);

	return 0;
}

int CDataSet::saveTraj(vector<CTraj*> saveT, QString saveP, string pc_driver) {

	//使属性表字段支持中文
	CPLSetConfigOption("SHAPE_ENCODING", "");
	GDALAllRegister();//注册所有的驱动

					  //调用对Shape文件读写的Driver
	GDALDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pc_driver.c_str());
	if (poDriver == NULL)
		return 12;        //Driver Error

						  // ====================================================
						  // output Traj

						  //创建数据源
	GDALDataset *poDs = poDriver->Create(saveP.toLatin1(), 0, 0, 0, GDT_Unknown, NULL);
	if (poDs == NULL)
		return 41;        //DataSource Creation Error

						  //创建图层Layer
						  //参数说明：新图层名称，坐标系，图层的几何类型，创建选项，与驱动有关
	OGRLayer *poLayer = poDs->CreateLayer("Traj", NULL, wkbLineString, NULL);
	if (poLayer == NULL) {
		GDALClose(poDs);
		return 42;        //Layer Creation Failed
	}

	//下面创建属性表，我们在属性表中创建两列数据即可
	OGRFieldDefn oFuId("UserID", OFTString);
	oFuId.SetWidth(50);
	poLayer->CreateField(&oFuId);

	OGRFieldDefn oFT("Traj", OFTString);
	oFT.SetWidth(100);
	poLayer->CreateField(&oFT);

	for (vector<CTraj*>::iterator iter = saveT.begin(); iter != saveT.end(); iter++) {

		//创建一个feature
		OGRFeature *poFeature;
		poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());  //GetLayerDefn()获取当前图层的属性表结构

																		 //创建一个OGRPoint对象
		OGRLineString line;

		string qs_types = "";

		vector<CStay> *_pcsSPs = (*iter)->getStayPoints();

		for (int _j = 0; _j < _pcsSPs->size(); ++_j) {
			CBase *_pB = _pcsSPs->at(_j).get_Base();
			qs_types += (*iter)->find_Location(_pB)->get_SPType_s() + " ";
			line.setPoint(_j, _pB->get_Lng(), _pB->get_Lat());
		}

		poFeature->SetField("UserID", (*iter)->getUsrID().c_str());
		poFeature->SetField("Traj", qs_types.c_str());

		poFeature->SetGeometry(&line);

		if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
			return 50 + poLayer->CreateFeature(poFeature);
		OGRFeature::DestroyFeature(poFeature);

	}

	GDALClose(poDs);

	return 0;
}


void CDataSet::analyse(){
    // NOTE: add code here...
    // not now
    return;
}

CBase* CDataSet::push_BaseStation(double dLng, double dLat, string sPid){
    CBase* _bs = this->find_BaseStation(dLng, dLat);
    if (_bs == NULL) {
        this->mhmapBaseS.insert(std::make_pair(CoorXY(dLng, dLat), CBase(sPid, dLng, dLat)));
        _bs = this->find_BaseStation(dLng, dLat);
    }
    return _bs;

}

CBase* CDataSet::find_BaseStation(double dLng, double dLat){
    auto _bs = mhmapBaseS.find(CoorXY(dLng, dLat));
    if (_bs == mhmapBaseS.end())
        return NULL;
    else
        return &_bs->second;
}



inline time_t CDataSet::readTime(string sTime) {
    QString qsTime=QString::fromStdString(sTime);
    QDateTime qTime = QDateTime::fromString(qsTime, mqsTfmt);
    return qTime.toTime_t();
}

inline string CDataSet::saveTime(time_t tTime) {
    QDateTime qTime = QDateTime::fromTime_t(tTime);
    return qTime.toString("yyyy-MM-dd hh:mm:ss").toStdString();
}
