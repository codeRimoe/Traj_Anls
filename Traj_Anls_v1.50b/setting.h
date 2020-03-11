#ifndef SETTING_H
#define SETTING_H

#include <QString>

#include <QDialog>
#include <QButtonGroup>
#include <QMouseEvent>

#include <string>
using std::string;

class MainWindow;

struct Para_Set{
    Para_Set():
    mps_nCol{0,1,2,3}, mps_nColTxt{""},
    mps_cSeg{','}, mps_bNoLine1{false},
    mps_qn64GS{1073741824}, mps_nPNo{1},
    mps_Para{240,240,6,22}{
        mps_qsFileName = QString::fromStdString("");
        mps_qsTime_fmt = QString::fromStdString("yyyy/M/d h:m:s");
        mps_qsTempPath = QString::fromStdString("");
    }
    QString show_paras(){
        QString _tempNoLine = QString::fromStdString("True");
        if (mps_bNoLine1)
            _tempNoLine = QString::fromStdString("False");

        return QString::fromStdString("=======================================\n- File Setting\n")+
               QString("Data File Path: %1\nTemporary Path: %2\n").arg(mps_qsFileName, mps_qsTempPath) +
               QString::fromStdString("=======================================\n- Normal Setting\n") +
               QString("UserID: %1;    Time: %2;    Lng: %3;    Lat: %4.\n").arg(
                    QString::fromStdString(mps_nColTxt[0]), QString::fromStdString(mps_nColTxt[1]),
                    QString::fromStdString(mps_nColTxt[2]), QString::fromStdString(mps_nColTxt[3])) +
               QString("Seg Signal: %1 ASCII(%2)\nSkip the 1st line: %3\nTime Formate: %4\n").arg(
                    QChar(mps_cSeg), QString::number(int(mps_cSeg)), _tempNoLine, mps_qsTime_fmt) +
               QString("Parameter: \nThreshold (min) of Home/Work: %1/%2\nDay time from %3 to %4\n").arg(
                    QString::number(int(mps_Para[0])), QString::number(int(mps_Para[1])), QString::number(int(mps_Para[2])), QString::number(int(mps_Para[3]))) +
               QString::fromStdString("=======================================\n- Processor Setting\n") +
               QString("File Mapping Grain: %1\nMulti-Processors Number:%2\n").arg(QString::number(long(mps_qn64GS)), QString::number(mps_nPNo)) +
               QString::fromStdString("=======================================");
    }

    int mps_nCol[4], mps_Para[4], mps_nPNo;
    qint64 mps_qn64GS, mps_qn64BS;
    string mps_nColTxt[4];
    char mps_cSeg;
    bool mps_bNoLine1;
    QString mps_qsFileName, mps_qsTime_fmt, mps_qsTempPath;
    //int mps_hT, mps_wT, mps_DTS, mps_DTE; ->mps_Para[4]
};

namespace Ui {
class Setting;
}

class Setting : public QDialog
{
    Q_OBJECT

public:
    explicit Setting(MainWindow *mw,QWidget *parent = 0);
    ~Setting();
    Para_Set get_para();
    QString get_fileName();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Ui::Setting *ui;
    QButtonGroup ButtonGroup;

    Para_Set mPara;
    MainWindow *mw_main;


    bool mb_move;
    QPoint mqp_startPoint;
    QPoint mqp_windowPoint;

private slots:
    void openFile();
    void readFile();
    void closeWin();
    void setPara();
    void previewTime();
    void reset();

};

#endif // SETTING_H
