#ifndef CDS_H
#define CDS_H

#include <vector>
#include <unordered_map>
#include <iomanip>

#include "CTraj.h"
#include "CBase.h"


#include <QString>
#include <QProgressBar>

using std::vector; using std::unordered_map;

// temperary xy
struct CoorXY{
	double x, y;
	CoorXY(double x_=0, double y_=0){
		x = x_, y = y_;
	}
	bool operator== (const CoorXY& p) const{
		return x == p.x && y == p.y;
	}
};

// define the hash function
struct hashFunc{
	int operator()(const CoorXY &_coor)const{
		double _X = _coor.x - (int)(_coor.x);
		double _Y = _coor.y - (int)(_coor.y);
		return (int)(_X * 1e8) + (int)(_Y * 1e4);
	}
};

struct StaticPara {
	string sTemp{ "" }, sUserID{ "" }, sEndTime{ "" };
	int nSBaseID{ 0 };
	vector<string> vsTemp;

	CTraj *ptTempTraj{ nullptr };
	CStay *psTempStay{ nullptr };
	CBase *pbTempBase{ nullptr };
};

class CDataSet{

public:
	CDataSet();
	~CDataSet();

private:
	vector<CTraj> mvTrajs;
    unordered_map<CoorXY, CBase, hashFunc> mhmapBaseS;
    QString mqsTfmt;

public:
    unordered_map<CoorXY, CBase, hashFunc>* getBS();
    void load_data(QString sFilePath, char*, int*, bool, int*, qint64, QString, QProgressBar*);
    int saveTxt(QString sFilePath, QString savename);
    int saveGeo(QString sFilePath, QString savename, QString fT);

	int saveTraj(CTraj*, QString, string);
	int saveTraj(vector<CTraj*>, QString, string);

	void analyse();
	int reload(QString path, QProgressBar* pProgressBar);

	CBase* push_BaseStation(double dLng, double dLat, string sPid);
	CBase* find_BaseStation(double dLng=0, double dLat=0);
	vector<CTraj>* getCTrajs();

protected:
    void process_input(char*, char*, int*, int*, StaticPara*);
    void mk_GDSet();
    time_t readTime(string);
    string saveTime(time_t tTime);
};

#endif //CDS_H
