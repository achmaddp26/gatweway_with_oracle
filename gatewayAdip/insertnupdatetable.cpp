#include "insertnupdatetable.h"
#include "ui_insertnupdatetable.h"

insertNUpdateTable::insertNUpdateTable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::insertNUpdateTable)
{
    ui->setupUi(this);
}

insertNUpdateTable::~insertNUpdateTable()
{
    delete ui;
}
