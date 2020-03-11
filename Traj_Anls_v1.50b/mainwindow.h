#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>

#include "setting.h"
#include "save.h"
#include "display.h"

#include "CDataSet.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setEn(int, bool);
    void reset(int);
    void save_res(int, QString);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void saveGeoTip(QString qsFilePath, QString savename, QString fT);

private:
    Ui::MainWindow *ui;
    Setting *mw_set_view;
    Save *mw_save;
    Display *mw_dsp;

    bool mb_move;
    QPoint mqp_startPoint;
    QPoint mqp_windowPoint;

    Para_Set mPara;
    CDataSet* mds_dataSet;




private slots:
    void set_para();
    void process();
    void reload();
    void save();
    void display();
    void about();
    void exit();
};

#endif // MAINWINDOW_H
