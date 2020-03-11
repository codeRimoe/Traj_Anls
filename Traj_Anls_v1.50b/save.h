#ifndef SAVE_H
#define SAVE_H

#include <QDialog>
#include <QMouseEvent>

class MainWindow;

namespace Ui {
class Save;
}

class Save : public QDialog
{
    Q_OBJECT

public:
    explicit Save(MainWindow *, QWidget *parent = 0);
    ~Save();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Ui::Save *ui;
    bool mb_move;
    QPoint mqp_startPoint;
    QPoint mqp_windowPoint;

    MainWindow *mw_main;

private slots:
    void save_b();
    void cancel_b();
};

#endif // SAVE_H
