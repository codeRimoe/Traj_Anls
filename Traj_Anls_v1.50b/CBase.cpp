#include "CBase.h"

CBase::CBase(string Pid, double Lng, double Lat):
    msPid(Pid), mdLng(Lng), mdLat(Lat){
}


CBase::~CBase(){
}

string CBase::get_Pid(){
	return msPid;
}

double CBase::get_Lng(){
	return mdLng;
}

double CBase::get_Lat(){
	return mdLat;
}

string CBase::outString() {
	return msPid + '\t' + std::to_string(mdLng) + '\t' + std::to_string(mdLat);
}
