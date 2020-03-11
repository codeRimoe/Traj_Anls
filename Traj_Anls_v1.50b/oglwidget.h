#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <vector>

#include <QWidget>
#include <QOpenGLWidget>

#include <gl/GLU.h>
#include <gl/GL.h>

#include "CDataSet.h"

using std::vector;

class OGLWidget : public QOpenGLWidget
{
public:
    OGLWidget(QWidget *parent = 0);
    ~OGLWidget();
    void set_ds(CDataSet *);
    void see_all();
	void see_traj();
    void mul_scale(double);
    void add_center(double Lng, double Lat);
    //void mul_vDg(double);
    //void add_split(int);

	void set_currentTraj(CTraj*);
	void set_selectedBase(CBase*);
	void set_selectedTrajs(vector<CTraj*>);

private:
    CDataSet *mp_ds;
    double md_r, md_pjLng, md_pjLat, md_dLng, md_dLat; //for user change
	double md_scale, md_view_dg;  //not suggest change by user
    int mn_up_down, mn_split_dg, mn_w, mn_h;
    bool mb_rClick;
	int mpn_sp[15];

	CTraj* mpt_currentTraj;
	CBase* mpb_selectedBase;
	vector<CTraj*> mv_selectedTrajs;

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

	void drawPoint(double, double, double, double);
	int get_GN(int);

};

#endif // OGLWIDGET_H
