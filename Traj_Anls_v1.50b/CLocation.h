#ifndef CLOCA_H
#define CLOCA_H
#include <string>

#include "CBase.h"

typedef enum _SPT {              // type of stay point
	Other = 0,
	Home = 1,
	Work = 2,
}SPT;

class CLocation{
public:
	CLocation(CBase *pBase, SPT cSPType = Other);
	~CLocation();

private:
	CBase *mpBase;
	int mnDay_times, mnNight_times;
	SPT mcSPType;                 // the type of stay point

public:
	int get_SPType();
	string get_SPType_s();
	void set_SPType(SPT SPType);
	void load_loc(string SPT_s, int day_time, int night_time);

	void addTime(int *nStay);
	void addNight(int nInT);
    void addDay(int nInT);
	int getDay();
	int getNight();
	CBase* get_Base();
	string outString();
};

#endif //CLOCA_H
