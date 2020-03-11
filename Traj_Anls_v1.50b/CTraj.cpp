#include "CTraj.h"

CTraj::CTraj(string UsrID, TJC TJClass):
	msUsrID(UsrID), mcTJClass(Oth){
}

CTraj::~CTraj(){
}

string CTraj::getUsrID(){
	return msUsrID;
}

vector<CStay>* CTraj::getStayPoints(){
	return &mvStayPoints;
}

unordered_map<int, CLocation>* CTraj::getLocation() {
	return &mmapLoc;
}

void CTraj::setTJClass(TJC TJClass){
	mcTJClass = TJClass;
	return;
}


CStay* CTraj::pushStayPoint(CBase* pBaseStation, time_t sST){
	//CStay _stayPoint(push_Location(pBaseStation), sST, sET);
	mvStayPoints.emplace_back(push_Location(pBaseStation), sST, sST);
	return &mvStayPoints.back();
}

CBase* CTraj::push_Location(CBase* pBaseStation) {
	CLocation* _loc = find_Location(pBaseStation);
    if (_loc == NULL) {
        this->mmapLoc.insert(std::make_pair(int(pBaseStation), CLocation(pBaseStation)));
	}
	return pBaseStation;
}

CLocation* CTraj::find_Location(CBase* pBaseStation) {
	auto _loc = mmapLoc.find(int(pBaseStation));
	if (_loc == mmapLoc.end())
		return NULL;
	else
		return &_loc->second;
}

void CTraj::label(int * n_para) {
	// NOTE: add code here...
	CLocation  *plDay = nullptr, *plDay2 = nullptr, *plNight = nullptr, *plAll = nullptr;
    int nDay{ -1 }, nDay2{ -1 }, nNight{ -1 }, nAll{ -1 }, n_noDay{1};
    n_noDay += (this->mvStayPoints.back().get_EndT() - this->mvStayPoints[0].get_StartT()) / 86401;
	for (auto _it = mmapLoc.begin(); _it != mmapLoc.end(); ++_it){
		CLocation *_pl = &_it->second;
		int _day = _pl->getDay(), _night = _pl->getNight();
		int _all = _day + _night;
		if (_all > nAll) {
			nAll = _all;
			plAll = _pl;
		}
			
		if (_day > nDay2) {
			if (_day > nDay) {
				nDay2 = nDay;
				plDay2 = plDay;
				nDay = _day;
				plDay = _pl;
			}
			else {
				nDay2 = _day;
				plDay2 = _pl;
			}	
		}
		if (_night > nNight) {
			nNight = _night;
			plNight = _pl;
		}
    }

    if (nNight > n_para[0] * n_noDay)
        plNight->set_SPType(Home);
    else
        plAll->set_SPType(Home);

    if (nDay >= n_para[1] * n_noDay) {
        if (plDay->get_SPType() == Home){
            if (nDay2 >= n_para[1] * n_noDay)
                plDay2->set_SPType(Work);
        }
        else
            plDay->set_SPType(Work);
    }

	return;
}

void CTraj::classify() {
	// TODO: add code here...
	// after appending this function, you should rewrite get_TJClass_s, outString
	// and update output and reload module
	mcTJClass = Oth;
	return;
}

string CTraj::get_TJClass_s() {
	// TODO: add code here...
	return "";
}

string CTraj::outString() {
	string _sTemp=msUsrID+":  ";
	for (auto _it = mvStayPoints.begin(); _it != mvStayPoints.end(); _it++) {
		CBase *_pB = _it->get_Base();
		_sTemp += find_Location(_pB)->get_SPType_s() + _pB->get_Pid() + " - ";
	}
	_sTemp += "END";
	return _sTemp;
}
