#include "CStay.h"
#include <string>
#include <sstream>
#include <iomanip>
CStay::CStay(CBase* pBase, time_t ttST, time_t ttET):
	mpBase(pBase), mttStartTime(ttST), mttEndTime(ttET){
}

CStay::~CStay(){
}

void CStay::get_StayT(int *nStay, int *nPara){
	// assume that no stay point last more that 1 day
    std::tm _tmTime;
    int nSHour{ 0 }, nEHour{ 0 };
    int ndS{nPara[2]}, ndE{nPara[3]};
    int nd{ int((mttEndTime - mttStartTime)/60) };
    if (nd == 0)
        nd = 1;
	localtime_s(&_tmTime, &mttStartTime);
    nSHour = (_tmTime.tm_hour - ndS + 24) % 24 - (ndE - ndS);      //TODO:the parameter should be able to custom

	localtime_s(&_tmTime, &mttEndTime);
    nEHour = (_tmTime.tm_hour - ndS + 24) % 24 - (ndE - ndS);

    if (nd < 0){
		return;
	}

	if (nSHour * nEHour >= 0) {
		if (nSHour >= 0)
			nStay[0] = nd;
		else
			nStay[1] = nd;
	}
	else {
		if (nSHour >= 0) {
            nEHour += (ndE - ndS), nSHour -= (24 - (ndE - ndS));
			double _dper = nEHour * 1.0 / (nEHour - nSHour);
            nStay[0] = int((1 - _dper) * nd);
            nStay[1] = int(_dper * nd);
		}
		else {
			double _dper = nEHour * 1.0 / (nEHour - nSHour);
            nStay[0] = int(_dper * nd);
            nStay[1] = int((1 - _dper) * nd);
		}
	}

	return ;
}

time_t CStay::get_StartT(){
    return mttStartTime;
}

time_t CStay::get_EndT(){
    return mttEndTime;
}

CBase* CStay::get_Base(){
	return mpBase;
}

void CStay::set_Base(CBase* pBase) {
	mpBase = pBase;
}

void CStay::set_EndT(time_t ttET){
	mttEndTime = ttET;
	return;
}
