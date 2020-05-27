#include "tableedit.h"
#include "ui_tableedit.h"
#include <QDebug>
#include <QThread>
#include <QCompleter>
tableedit::tableedit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tableedit)


{

    ui->setupUi(this);


}

tableedit::~tableedit()
{
    delete ui;
}

void tableedit::on_buttonBox_accepted()
{


    GbfrRF= ui->line_RFedit->text();
    GbfrLine=ui->line_LineEdit->text();
    GbfrZone=ui->line_ZoneEdit->text();
    GbfrPlant=ui->line_PlantEdit->text();
    GbfrDevice=ui->line_DeviceEdit->text();
    GbfrMachine=ui->line_MachineEdit->text();
    GbfrProcess=ui->line_ProcessEdit->text();
    GbfrSubzone=ui->line_SubzoneEdit->text();
    gate=1;


}
void tableedit::setData( QString bfrRF,QString bfrMachine,QString bfrPlant,QString bfrLine,QString bfrProcess,QString bfrZone,QString bfrSubzone,QString bfrDevice){
    ui->EditTableWidget->setCurrentIndex(0);
    ui->line_RFedit->setText(bfrRF);
    ui->line_LineEdit->setText(bfrLine);
    ui->line_ZoneEdit->setText(bfrZone);
    ui->line_PlantEdit->setText(bfrPlant);
    ui->line_DeviceEdit->setText(bfrDevice);
    ui->line_MachineEdit->setText(bfrMachine);
    ui->line_ProcessEdit->setText(bfrProcess);
    ui->line_SubzoneEdit->setText(bfrSubzone);
}

void tableedit::radiobox_filter(QSqlDatabase db)
{

    ui->table_filter_gateway->setRowCount(0);
   int row;
//    bool cek;
   QStringList listt;

    QSqlQuery query(db);

    query.prepare(tr("SELECT REPEATER FROM SMA_ITX_M_REPEATER  ").arg(ui->lineEdit->text()));
    query.exec();

    while(query.next()){
        row=ui->table_filter_gateway->rowCount();
        ui->table_filter_gateway->insertRow(row);
        QTableWidgetItem *item1=new QTableWidgetItem(query.value(0).toString());

        listt<<query.value(0).toString();



        ui->table_filter_gateway->setItem(row,0,item1);


        }
    qDebug()<<listt;
    QCompleter *cmp= new QCompleter(listt,this);
    cmp->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEdit->setCompleter(cmp);




//    for(int i = 0; query.next(); i++){
//          // Insert row
//          ui->table_filter_gateway->insertRow(i);
//          /* Set the id column in the first taking it from the result of the SQL-query.
//           * This column will be hidden
//           * */
//          QTableWidgetItem *item =  new QTableWidgetItem(query.value(0).toString());
//           item->data(Qt::CheckStateRole);
//          item->setCheckState(Qt::Unchecked);
//          ui->table_filter_gateway->setItem(i,0,item);

          // Create an element, which will serve as a checkbox


          /* Check on the status of odd if an odd device,
           * exhibiting state of the checkbox in the Checked, Unchecked otherwise
           * */
//          if(query.value(0).toInt() == 1){
//              item->setCheckState(Qt::Checked);
//          } else {

//          }
          // Set the checkbox in the second column
//          ui->table_filter_gateway->setItem(i,1, item);
          // Next, pick up all the data from a result set in other fields
//          ui->table_filter_gateway->setItem(i,2, new QTableWidgetItem(query.value(2).toString()));
//          ui->table_filter_gateway->setItem(i,3, new QTableWidgetItem(query.value(3).toString()));
//          ui->table_filter_gateway->setItem(i,4, new QTableWidgetItem(query.value(4).toString()));
//      }


}

void tableedit::on_buttonBox_2_accepted()
{

}



