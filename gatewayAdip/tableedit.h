#ifndef TABLEEDIT_H
#define TABLEEDIT_H

#include <QDialog>
#include "console.h"
#include<QStackedWidget>
#include <QTableWidgetItem>
#include <QGridLayout>
#include <QSqlDatabase>
#include <QSqlQuery>
//#include"mainwindow.h"
//#include "ui_mainwindow.h"
namespace Ui {
class tableedit;
}

class tableedit : public QDialog
{
    Q_OBJECT

public:
    explicit tableedit(QWidget *parent = nullptr);
    ~tableedit();
    QString GbfrRF,GbfrMachine, GbfrPlant, GbfrLine, GbfrProcess, GbfrZone, GbfrSubzone, GbfrDevice;
    int gate;
    int i;
    int arow,bcolumn;


public slots:
    void setData( QString bfrRF,QString bfrMachine,QString bfrPlant,QString bfrLine,QString bfrProcess,QString bfrZone,QString bfrSubzone,QString bfrDevice);
     void radiobox_filter(QSqlDatabase db);

private slots:


    void on_buttonBox_accepted();




    void on_buttonBox_2_accepted();

private:
    Ui::tableedit *ui;
//    tableedit TableEdit;
    QStackedWidget *stk;

};

#endif // TABLEEDIT_H
