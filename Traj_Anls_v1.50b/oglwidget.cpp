
#include "oglwidget.h"

#include "ogrsf_frmts.h"
#include <list>
#include <cmath>

#define PI 3.14159
using namespace std;


OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      md_r(6371), md_scale(3),
      md_pjLng(0),md_pjLat(90),
      md_view_dg(20),
      mn_split_dg(18),
      mn_up_down(1),
      mn_w(0), mn_h(0),
      mb_rClick(false),
	mpt_currentTraj(nullptr),
	mpb_selectedBase(nullptr),
	mpn_sp{ 18, 24, 36, 72, 120, 180, 360,
	720, 1080, 1440, 2160, 3600, 7200, 10800, 21600 }
{
this->setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);
mv_selectedTrajs.clear();
}

OGLWidget::~OGLWidget(){
}

void OGLWidget::initializeGL(){
    glClearColor(0,0,0,1);      // set bgcolor
    glEnable(GL_DEPTH_TEST);    // z-depth
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
}

void OGLWidget::drawPoint(double lng, double lat, double h=0, double show_dis=1) {
	double dlng = abs(lng - md_pjLng), dlat = abs(lat - md_pjLat);
	dlng = min(dlng, 360 - dlng);
	if (show_dis == -1)
		show_dis = 360;
	else
    	show_dis *= max(md_dLng, md_dLat);
	if ((dlng > show_dis) || (dlat > show_dis))
		return;

	glVertex3f((md_r + h) * cos(lng * PI / 180) * cos(lat * PI / 180),
		       (md_r + h) * sin(lng * PI / 180) * cos(lat * PI / 180),
               (md_r + h) * sin(lat * PI / 180));
}

int OGLWidget::get_GN(int n) {
	int i = 0;
	while (mpn_sp[i] < n)
		i++;
	return mpn_sp[i];
}

void OGLWidget::paintGL(){

    if (mn_h <= mn_w) {
        if (md_dLng > 180){
            md_dLng = 180;
            md_dLat = 180.0 / mn_w * mn_h;
        }
        if (md_dLat < 0.1){
            md_dLat = 0.1;
            md_dLng = 0.1 / mn_h * mn_w;
        }
	}
	else{
        if (md_dLat > 180){
            md_dLat = 180;
            md_dLng = 180.0 / mn_h * mn_w;
        }
        if (md_dLng < 0.1){
            md_dLng = 0.1;
            md_dLat = 0.1 / mn_w *mn_h;
        }

	}

	mn_split_dg = 360 / min(md_dLng, md_dLat);
	mn_split_dg = get_GN(mn_split_dg);

	int _s = mn_split_dg <= 360 ? -1 : 3;

	//double d_h = max(sin(md_dLat * PI / 360), sin(md_dLng * PI / 360) * mn_h / mn_w);
	double d_h = sin(md_dLat * PI / 360);
	md_scale = d_h / tan(md_view_dg * PI / 360) + 1;


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(md_view_dg, (float)mn_w/mn_h, md_r * (md_scale - 1)/10.0, md_r * md_scale -  md_r / md_scale);
    double d_split_dg = 180.0 / mn_split_dg;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	
    gluLookAt(md_scale * md_r * cos(md_pjLng * PI / 180) * cos(md_pjLat * PI / 180),
		      md_scale * md_r * sin(md_pjLng * PI / 180) * cos(md_pjLat * PI / 180),
		      md_scale * md_r * sin(md_pjLat * PI / 180),
              0,0,0,0,0,mn_up_down);
	
	

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLineWidth(1);
    int _t1 = 0;
    for(double _lng=-180; _t1<mn_split_dg*2; _lng += d_split_dg, ++_t1){
        glBegin(GL_LINE_STRIP);{
            glColor3f(0.0, 0.0, 1.0);
            int _t2 = 0;
            for(double _lat=-90; _t2<=mn_split_dg; _lat+=d_split_dg, ++_t2){
				drawPoint(_lng, _lat, 0, _s);
            }
        }glEnd();
    }

    int _t2 = 0;
    for(double _lat=-90; _t2<=mn_split_dg; _lat += d_split_dg,++_t2){
        glBegin(GL_LINE_STRIP);{
            glColor3f(0.0, 0.0, 1.0);
            int _t1 = 0;
            for(double _lng=-180; _t1<=mn_split_dg*2; _lng+=d_split_dg, ++_t1){
				drawPoint(_lng, _lat, 0, _s);
            }
        }glEnd();
    }

	glPointSize(3);
    glBegin( GL_POINTS );{
        glColor3f(0.8, 0.0, 0.0);
        auto _bs = mp_ds->getBS();
        for (auto _it = _bs->begin(); _it != _bs->end(); ++_it) {
            double _lng = _it->second.get_Lng();
            double _lat = _it->second.get_Lat();
			drawPoint(_lng, _lat, 0.01, 1);
        }
    }
    glEnd();

	for (vector<CTraj*>::iterator iter = mv_selectedTrajs.begin(); iter != mv_selectedTrajs.end(); iter++) {
		glLineWidth(3);
		glBegin(GL_LINE_STRIP); {
			glColor3f(0.0, 0.8, 0.0);
			vector<CStay>* vs_stay = (*iter)->getStayPoints();
			for (auto _it = vs_stay->begin(); _it != vs_stay->end(); ++_it) {
				CBase* _pb = _it->get_Base();
				double _lng = _pb->get_Lng();
				double _lat = _pb->get_Lat();
				drawPoint(_lng, _lat, 0.02, -1);
			}
		}
		glEnd();
	}

	if (mpt_currentTraj != nullptr){
		glLineWidth(4);
		glBegin(GL_LINE_STRIP); {
			glColor3f(0.0, 0.8, 0.8);
			vector<CStay>* vs_stay = mpt_currentTraj->getStayPoints();
			for (auto _it = vs_stay->begin(); _it != vs_stay->end(); ++_it) {
				CBase* _pb = _it->get_Base();
				double _lng = _pb->get_Lng();
				double _lat = _pb->get_Lat();
				drawPoint(_lng, _lat, 0.03, -1);
			}
		}
		glEnd();

		glPointSize(6);
		glBegin(GL_POINTS); {
			glColor3f(0.8, 0.0, 0.8);
			vector<CStay>* vs_stay = mpt_currentTraj->getStayPoints();
			for (auto _it = vs_stay->begin(); _it != vs_stay->end(); ++_it) {
				CBase* _pb = _it->get_Base();
				double _lng = _pb->get_Lng();
				double _lat = _pb->get_Lat();
				drawPoint(_lng, _lat, 0.04, -1);
			}
		}
		glEnd();
	}

	if (mpb_selectedBase != nullptr) {
		glPointSize(8);
		glBegin(GL_POINTS); {
			glColor3f(0.8, 0.8, 0.0);
			double _lng = mpb_selectedBase->get_Lng();
			double _lat = mpb_selectedBase->get_Lat();
			drawPoint(_lng, _lat, 0.05, -1);
		}
		glEnd();
	}
}

void OGLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    mn_w = w;
    mn_h = h;

}

void OGLWidget::see_all(){
    auto _bs = mp_ds->getBS();
    double d_max_lng = -360, d_min_lng = 360;
    double d_max_lat = -360, d_min_lat = 360;
    double d_lng = 0, d_lat = 0;
    for (auto _it = _bs->begin(); _it != _bs->end(); ++_it) {
        d_lng = _it->second.get_Lng();
        d_lat = _it->second.get_Lat();
        if(d_lng>d_max_lng)
            d_max_lng = d_lng;
        if(d_lng<d_min_lng)
            d_min_lng = d_lng;
        if(d_lat>d_max_lat)
            d_max_lat = d_lat;
        if(d_lat<d_min_lat)
            d_min_lat = d_lat;
    }
    if (d_lng*d_lat == 0)
        return;
    d_lng = d_max_lng-d_min_lng;
    d_lat = d_max_lat-d_min_lat;
    if (d_lng*d_lat == 0)
        return;

    md_pjLng = (d_max_lng + d_min_lng) / 2;
    md_pjLat = (d_max_lat + d_min_lat) / 2;
    mn_up_down = 1;
    md_view_dg = 5;

	//max the window
	if (d_lng > d_lat)
		d_lat = d_lng / mn_w * mn_h;
	else
		d_lng = d_lat / mn_h * mn_w;

	d_lng *= 1.05;
	d_lat *= 1.05;
	md_dLng = d_lng;
	md_dLat = d_lat;
}

void OGLWidget::see_traj() {
	if (mpt_currentTraj == nullptr) {
		return;
	}
	auto _bs = mpt_currentTraj->getLocation();
	double d_max_lng = -360, d_min_lng = 360;
	double d_max_lat = -360, d_min_lat = 360;
	double d_lng = 0, d_lat = 0;
	for (auto _it = _bs->begin(); _it != _bs->end(); ++_it) {
		CBase* _pb = _it->second.get_Base();
		d_lng = _pb->get_Lng();
		d_lat = _pb->get_Lat();
		if (d_lng>d_max_lng)
			d_max_lng = d_lng;
		if (d_lng<d_min_lng)
			d_min_lng = d_lng;
		if (d_lat>d_max_lat)
			d_max_lat = d_lat;
		if (d_lat<d_min_lat)
			d_min_lat = d_lat;
	}
	if (d_lng*d_lat == 0)
		return;
	d_lng = d_max_lng - d_min_lng;
	d_lat = d_max_lat - d_min_lat;
	if (d_lng*d_lat == 0)
		return;

	md_pjLng = (d_max_lng + d_min_lng) / 2;
	md_pjLat = (d_max_lat + d_min_lat) / 2;
	mn_up_down = 1;
	md_view_dg = 5;

	//max the window
	if (d_lng > d_lat)
		d_lat = d_lng / mn_w * mn_h;
	else
		d_lng = d_lat / mn_h * mn_w;

	d_lng *= 1.05;
	d_lat *= 1.05;
	md_dLng = d_lng;
	md_dLat = d_lat;
}

void OGLWidget::set_currentTraj(CTraj* pt_currentTraj) {
	mpt_currentTraj = pt_currentTraj;
}

void OGLWidget::set_selectedBase(CBase* pb_selectedBase) {
	mpb_selectedBase = pb_selectedBase;
}

void OGLWidget::set_selectedTrajs(vector<CTraj*> v_selectedTrajs) {
	mv_selectedTrajs = v_selectedTrajs;
}

void OGLWidget::set_ds(CDataSet *p_ds){
    mp_ds=p_ds;
}

void OGLWidget::mul_scale(double scale){
    //scale *= md_scale;
    //md_scale= scale < 1 ? 1 : scale;
	md_dLng *= scale;
	md_dLat *= scale;
    this->update();
}

void OGLWidget::add_center(double lng, double lat){
	//this->geometry().width()
	lng *= md_dLng / mn_w;
	lat *= md_dLat / mn_h;

    md_pjLng -= mn_up_down * lng;
    md_pjLat += mn_up_down * lat;

    if (md_pjLat<-90){
        md_pjLat = - 180 - md_pjLat;
        md_pjLng += 180;
        mn_up_down *= -1;
    }
    else if (md_pjLat>90){
        md_pjLat = 180 - md_pjLat;
        md_pjLng += 180;
        mn_up_down *= -1;
    }
	int n_lng = int(md_pjLng);
	md_pjLng += (n_lng + 540) % 360 - 180 - n_lng;
    this->update();
}

/*
void OGLWidget::mul_vDg(double wdg){
    wdg*=md_view_dg;
    if ((wdg>1/3600) && (wdg<90))
        md_view_dg = wdg;
    this->update();
}

void OGLWidget::add_split(int num){
    mn_split_dg += num;
    if (mn_split_dg < 18)
        mn_split_dg = 18;
    this->update();
}
*/
