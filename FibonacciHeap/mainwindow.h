#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "heap.h"

#define INSERT_TYPE 1
#define DELETEMIN_TYPE 2
#define DECREASE_TYPE 3
#define DELETE_TYPE 4
#define UNION_TYPE 5

QT_BEGIN_NAMESPACE
namespace Ui {
class mainwindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    NODES nodes;
    LINES lines;
    void BrowserLog(string message);

private slots:
    void on_Insert_button_clicked();
    void on_DeleteMin_button_clicked();
    void on_Decrease_button_clicked();
    void on_Delete_button_clicked();
    void on_Union_button_clicked();

    void on_Help_button_clicked();

    void on_About_button_clicked();

    void on_Load_button_clicked();

    void on_Save_button_clicked();

private:
    Ui::mainwindow *ui;
    void paintEvent(QPaintEvent *event) override;
    int heapid, para1, para2;
    void GetParameter();
    void UpdateWidget();
};
#endif // MAINWINDOW_H
