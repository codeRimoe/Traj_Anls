#include "CLocation.h"

CLocation::CLocation(CBase *pBase, SPT cSPType):
	mpBase(pBase), mcSPType(cSPType), mnDay_times(0), mnNight_times(0){
}


CLocation::~CLocation(){
}

int CLocation::get_SPType() {
	return mcSPType;
}

string CLocation::get_SPType_s() {
	switch (mcSPType) {
	case Other: return "+"; break;
	case Home: return "H"; break;
	case Work: return "W"; break;
	default: return "N"; break;
	}
}

void CLocation::set_SPType(SPT cSPType) {
	mcSPType = cSPType;
}

void CLocation::addTime(int *nStay) {  //0-Night; 1-Day
	mnNight_times += nStay[0];
	mnDay_times += nStay[1];
}

void CLocation::addNight(int nInT){
	mnNight_times += nInT;
}

void CLocation::addDay(int nInT){
	mnDay_times += nInT;
}

int CLocation::getDay(){
	return mnDay_times;
}

int CLocation::getNight(){
	return mnNight_times;
}

CBase* CLocation::get_Base() {
	return mpBase;
}

string CLocation::outString() {
	return mpBase->get_Pid() + "\t" + get_SPType_s() + "\t" + std::to_string(mnDay_times) + "\t" + std::to_string(mnNight_times);
}

void CLocation::load_loc(string SPT_s, int day_time, int night_time) {
	if (SPT_s == "+")
		mcSPType= Other;
	else if (SPT_s == "W")
		mcSPType = Work;
	else if (SPT_s == "H")
		mcSPType = Home;
	else
		mcSPType = Other;
	mnDay_times = day_time;
	mnNight_times = night_time;
}