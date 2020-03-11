#ifndef DISPLAY_H
#define DISPLAY_H

#include <QDialog>
#include <QPoint>
#include <QMouseEvent>
#include <QWheelEvent>
#include <map>

#include "oglwidget.h"
#include "CDataSet.h"
#include "ogrsf_frmts.h"
#include "gdal.h"
#include "gdal_priv.h"

using std::map;

namespace Ui {
class Display;
}

class Display : public QDialog
{
    Q_OBJECT

public:
    explicit Display(CDataSet *cd, QWidget *parent = 0);
    ~Display();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    Ui::Display *ui;
    CDataSet *mp_ds;

	map<QString, CTraj*> mm_selected_user;

    bool mb_move;
    QPoint mqp_startPoint;

	CTraj* mpt_currentTraj;
	CBase* mpb_selectedBase;

	vector<CLocation> mpl_pre;


public slots:
    void see_all();
	void see_traj();
	void show_selected_traj();
	
	vector<CTraj*> get_selectedTrajs();
	CTraj* get_currentTraj();
	CBase* get_selectedBase();
	

private slots:
    void loadUserList(int);
    void loadSelectedUser();
	void loadSelectedTraj();
	void showDetail();
	void clear_traj();
	void export_traj();
	

};

#endif // DISPLAY_H
