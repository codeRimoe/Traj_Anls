#ifndef CSTAY_H
#define CSTAY_H
#include <ctime>
#include "CLocation.h"

class CStay{
public:
	CStay(CBase* pBase, time_t sST, time_t sET);
	~CStay();

private:
	CBase* mpBase;         // the key of Base station in map
	time_t mttStartTime, mttEndTime;     // the value type can be a Date, depends on implement
	
public:
    void get_StayT(int* nStay, int* nPara);                   // the value type can be a Date, depends on implement
    time_t get_StartT();
    time_t get_EndT();
	CBase* get_Base();
	void set_Base(CBase* pBase);
    void set_EndT(time_t sET);
};

#endif //CSTAY_H
