#ifndef CBASE_H
#define CBASE_H

#include <string>

using std::string;

class CBase{
public:
	CBase(string Pid, double Lng, double Lat);
	~CBase();

private:
	string msPid;          // other features such as MAC/LC
	double mdLng, mdLat;   // the spatial feature (geo-coordinate) of a base sataion

public:
	string get_Pid();
	double get_Lng();
	double get_Lat();
	string outString();
};

#endif //CBASE_H
