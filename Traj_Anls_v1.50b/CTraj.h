#ifndef CTRAJ_H
#define CTRAJ_H

#include <string>
#include <vector>
#include <unordered_map>
#include "CStay.h"

using std::string; using std::vector; using std::unordered_map;

typedef enum _TJC{                   // the class of traj
	Oth = 0,
	HW = 1,
	HWHW = 2,
} TJC;

class CTraj{
public:
	CTraj(string UsrID, TJC TJClass=Oth);
	~CTraj();

private:
	string msUsrID;
	vector<CStay> mvStayPoints;
	unordered_map<int, CLocation> mmapLoc;
	TJC mcTJClass;

public:
	string getUsrID();
	vector<CStay>* getStayPoints();
	unordered_map<int, CLocation>* getLocation();
	void setTJClass(TJC TJClass);
	CStay* pushStayPoint(CBase* baseStation, time_t sST);

    void label(int*);
	void classify();
	CBase* push_Location(CBase* pBaseStation);
	CLocation* find_Location(CBase* pBaseStation);
	string outString();

	string get_TJClass_s();
};

#endif //CTRAJ_H
