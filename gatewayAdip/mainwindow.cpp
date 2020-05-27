#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include <QtSql>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QSerialPort>
#include <QMenu>
#include <QAction>
#include <QDesktopWidget>
#include "ui_tableedit.h"
#include <QDateTime>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //set();
    setWindowTitle(tr("Gateway Internex"));
    createActions();
    createTrayIcon();
    console = new Console;

    console->setEnabled(false);

//    ui->toolBar->addWidget(console);
    ui->Tempat_Console->insertWidget(0,console);
    ui->Tempat_Console->setCurrentIndex(0);
    console->setEnabled(true);


    RFlast="";
    btnLast="";
    startRFbtn=0;
    startmemori=0;

    QLibrary *hello_lib = NULL;
    QString Loc = qApp->applicationDirPath();
    QString Loc_Setting = (tr("%1/%2").arg(Loc).arg("instantclient_11_2/oci.dll"));
    hello_lib = new QLibrary(Loc_Setting);


        hello_lib->load();
        if (!hello_lib->isLoaded())
        {
            qDebug("load Oracle failed!\n");
            //print_data("load plugin failed\r\n");
        }
        else
        {
            qDebug("load Oracle success!\n");
            //print_data("load plugin success\r\n");

        }



    connect(trayIcon,&QSystemTrayIcon::activated,this,&MainWindow::iconActivated);

    //QLibrary mysqllib("C:/Users/achma/Documents/build-gatewayAdip-Desktop_Qt_5_13_2_MinGW_64_bit-Debug/libmysql.dll");
    time_now=QTime::currentTime();
    rows=0;
    autorunAps();
    connect(serial,static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),this,&MainWindow::cekCOM);

    //ui->table_simulasi->setContextMenuPolicy(Qt::CustomContextMenu);
    //connect(ui->table_simulasi,SIGNAL(customContextMenuRequested( QPoint )),this,SLOT(ProvideContextMenu( QPoint )));

     QTimer *timer1= new QTimer(this);
     connect(timer1, &QTimer::timeout,[&](){
         if(ui->btn_Connect_Comunication->text()=="Disconnect"){
                 processRead();


         }

     });
     timer1->start(100);
     QTimer *timer2= new QTimer(this);
     connect(timer2, &QTimer::timeout,[&](){
//         QSqlQuery querry(db);
//         if(!db.open()){
//         open_DB();

//         }

     });
     timer2->start(5000);

     trayIcon->setIcon(QIcon(":/internexLOgo.png"));
     trayIcon->show();
     trayIcon->setVisible(true);
     //ambil nilai tabel simulasi

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_table_simulasi_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->table_simulasi->indexAt(pos);
    if(!index.isValid()){
        return;
    }
    else{
    QPoint globalPos = ui->table_simulasi->mapToGlobal(pos);

       // Create menu and insert some actions
       QMenu myMenu;
       //myMenu.addAction("Add Button", this, SLOT(addItem()));
       myMenu.addAction("Update Button",  this, SLOT(updateItem()));
       myMenu.addAction("Delete Button",  this, SLOT(deleteItem()));
       // Show context menu at handling position
       myMenu.exec(globalPos);

//       myMenu.exec(QCursor::pos());
//       myMenu.exec(ui->table_simulasi->viewport()->mapToGlobal(pos));

    }
}


void MainWindow::updateItem()
{


    QModelIndex currentIndex4=ui->table_simulasi->currentIndex();


    bfrRF=ui->table_simulasi->item(currentIndex4.row(),0)->text();
    bfrMachine=ui->table_simulasi->item(currentIndex4.row(),1)->text();
    bfrPlant=ui->table_simulasi->item(currentIndex4.row(),2)->text();
    bfrLine=ui->table_simulasi->item(currentIndex4.row(),3)->text();
    bfrProcess=ui->table_simulasi->item(currentIndex4.row(),4)->text();
    bfrZone=ui->table_simulasi->item(currentIndex4.row(),5)->text();
    bfrSubzone=ui->table_simulasi->item(currentIndex4.row(),6)->text();
    bfrDevice=ui->table_simulasi->item(currentIndex4.row(),7)->text();

    TableEdit.setData( bfrRF,bfrMachine, bfrPlant, bfrLine, bfrProcess, bfrZone, bfrSubzone, bfrDevice);

   QByteArray r;

  //print_data(r.append(bfrPlant)+"\r\n");
   //tabelEdit->EditTableWidget->setCurrentIndex(0);
    int resultTableEdit=TableEdit.exec();
    if(resultTableEdit==QDialog::Accepted){
        update_table_simulasi();
        initTable_Simulasi();
        print_data("update Button\r\n");
    }


}

void MainWindow::deleteItem()
{


    QModelIndex currentIndex4=ui->table_simulasi->currentIndex();
    bfrRF=ui->table_simulasi->item(currentIndex4.row(),0)->text();
    bfrMachine=ui->table_simulasi->item(currentIndex4.row(),1)->text();
    bfrPlant=ui->table_simulasi->item(currentIndex4.row(),2)->text();
    bfrLine=ui->table_simulasi->item(currentIndex4.row(),3)->text();
    bfrProcess=ui->table_simulasi->item(currentIndex4.row(),4)->text();
    bfrZone=ui->table_simulasi->item(currentIndex4.row(),5)->text();
    bfrSubzone=ui->table_simulasi->item(currentIndex4.row(),6)->text();
    bfrDevice=ui->table_simulasi->item(currentIndex4.row(),7)->text();

    delete_item_table_simulasi();

    print_data("Deleted button \r\n");


}

void MainWindow::update_table_simulasi()
{
    QString RFcek_table;
    QSqlQuery query(db);
    query.prepare(tr("select id,rf_address from sma_itx_m_device_list where rf_address like '%1'").arg(TableEdit.GbfrRF));
    query.exec();
    while (query.next()){
      RFcek_table=query.value(0).toString();

    }
    //qDebug()<<"RFcektable"<<RFcek_table;
    query.prepare(tr("UPDATE sma_itx_m_device_list SET RF_ADDRESS='%1',MACHINE_ID='%2',PLANT_ID='%3',LINE_ID='%4',PROCESS_ID='%5',ZONE_ID=%6,SUBZONE_ID=%7 WHERE ID=%8").arg(TableEdit.GbfrRF).arg(TableEdit.GbfrMachine).arg(TableEdit.GbfrPlant).arg(TableEdit.GbfrLine).arg(TableEdit.GbfrProcess).arg(TableEdit.GbfrZone).arg(TableEdit.GbfrSubzone).arg(RFcek_table));
    query.exec();

}

void MainWindow::delete_item_table_simulasi()
{
    QString RFcek_table;
    QSqlQuery query(db);
    query.prepare(tr("select id,rf_address from sma_itx_m_device_list where rf_address like '%1'").arg(bfrRF));
    query.exec();
    while (query.next()){
      RFcek_table=query.value(0).toString();

    }

    query.prepare(tr("delete from sma_itx_m_device_list where id='%1'").arg(RFcek_table));
    query.exec();
    initTable_Simulasi();

}

void MainWindow::addItem()
 {  print_data("add");

//        static int i = 0;
//        table->addItem(QString::number(++i));
 }
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason==QSystemTrayIcon::Trigger)
    {
        show();
    }
}

void MainWindow::changeEvent(QEvent* event)
{
   if (event->type() == QEvent::WindowStateChange){
       if (isMinimized() == true){
               //setWindowFlags(windowFlags() | Qt::Tool);
            if(trayIcon->isVisible()){
               hide();

             }

        }

   }

      return QMainWindow::changeEvent(event);
 }



void MainWindow::createActions()
{
    QIcon(QPixmap("internex.png"));
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

    restoreAction = new QAction(tr("&Show"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::notifikasi_panggilan(QString Address,QString button){
    QString notifikasi;
    QByteArray pesan;
    if(button=="01"){
         notifikasi = tr("Andon Call Material, from: %1 %2 \r\n").arg(Address).arg(button);
         pesan.append(notifikasi);
         print_data(pesan);
         pesan.clear();
     }
     if(button=="02"){
          notifikasi = tr("Andon Call Cancel/Stop, from: %1 %2 \r\n").arg(Address).arg(button);
          pesan.append(notifikasi);
          print_data(pesan);
          pesan.clear();
     }
     if(button=="03"){
          notifikasi = tr("Andon Call Machine, from: %1 %2 \r\n").arg(Address).arg(button);
          pesan.append(notifikasi);
          print_data(pesan);
          pesan.clear();
     }

     if(button=="04"){
          notifikasi = tr("Andon Call Quality, from: %1 %2 \r\n").arg(Address).arg(button);
          pesan.append(notifikasi);
          print_data(pesan);
          pesan.clear();
     }

//    pesan.append(notifikasi);
//    print_data(pesan);
}

void MainWindow::processRead(){
    QSqlQuery query(db);
    QString time_start;
    int i;
    QByteArray incomingData;
    QByteArray data_clean;
//  QTime Time_now= QTime::currentTime();

    while(serial->bytesAvailable() >1){
            //Data received
          incomingData.append(serial->readAll());
          filterdataWho(incomingData);
          //qDebug()<<"raw"<<incomingData;
          for( i=0; i<incomingData.size();i++){
             if(incomingData[i]=='D'){
                 break;
             }
          }

          if(i<incomingData.size()){
            incomingData.remove(0,i);
          }


          QString input_str = (QString) incomingData;
          input_str.replace("\r","@");
          for(int i=0; i<incomingData.size();i++){
               QString data = (QString)input_str[i];
               if((data!="@")){
                    bfr.append(incomingData[i]);


               }
               else{
                    data_clean= cleardata(bfr);
                    bfr.clear();

               }

          }

    }

}

void MainWindow::Insert_Data(QString DataWHO){
     //print_data("WHO PROCESS\r\n");
     QStringList WhoInput;

     WhoInput = DataWHO.split("#");

     qDebug() << WhoInput.at(0);
     qDebug() << WhoInput.at(1);
     qDebug() << WhoInput.at(2);
     qDebug() << WhoInput.at(3);
     qDebug() << WhoInput.at(4);
     qDebug() << WhoInput.at(5);

     QSqlQuery query(db);
     query.prepare(tr("UPDATE SMA_ITX_M_REPEATER set LAST_ACTION=systimestamp WHERE REPEATER='%1'").arg(WhoInput.at(2)));
     query.exec();
}

void MainWindow::filterdataWho(QByteArray dataWho){
    int i;
    QByteArray data2Who;
    QStringList list_input;
    //qDebug()<<"haha"<<dataWho;
    for(i=0; i<dataWho.size();i++){
        if(dataWho[i]=='W'){
            break;
        }
    }

    if(i<dataWho.size()){
       dataWho.remove(0,i);

       //data2Who= dataWho.left(2);


    }
    for (i=dataWho.size();i>=0;i--) {
        if(dataWho[i]=='1'){
            break;
        }
    }
    if(i<dataWho.size()){
       dataWho.remove(i+1,dataWho.size());

        if((dataWho[0]=='W')&&(dataWho[1]=='H')&&(dataWho[2]=='O')){
            if(dataWho.size()==25){
                print_data(dataWho+"\r\n");
            }

 }


//        print_data(dataWho+"\r\n");





        //Insert_Data((QString)dataWho);

    }
    QString input_str = QString::fromStdString(dataWho.toStdString());
     input_str.replace("\n","");
    if (input_str.startsWith("WHO"))
    {    qDebug()<<"haha"<<dataWho;

        list_input= input_str.split('#');



}
}

QByteArray MainWindow::cleardata(QByteArray datain){ //filter

    int i;
            QStringList List_input;
            //filterdataWho(datain);
            //find start text
            for(i=0; i<datain.size();i++){
                if((datain[i]=='D')&&(datain[i+1]=='A')&&(datain[i+2]=='T')){
                    break;
                }
            }

            if(i<datain.size()){
               datain.remove(0,i);
            }

    QString input_str = QString::fromStdString(datain.toStdString());
            input_str.replace("\n","");


            if(input_str.startsWith("DAT")){

                 if(datain.size()>9){

                      qDebug() << "RECEIVED DATA:" << input_str;

                      List_input = input_str.split("#");

                       // console->putData(datain);
                        if((List_input.size()>=3)){

                            if((List_input.at(1).size()<8) || (List_input.at(2).size()<4))
                          {
                                //inisial button dan Rf
                                RF_SAddress=List_input.at(1);
                                //BtnS=List_input.at(2);
                                if(List_input.at(2)=="01"){
                                    BtnS="01";
                                }
                                else if(List_input.at(2)=="02"){
                                    BtnS="02";
                                }
                                else if(List_input.at(2)=="04"){
                                    BtnS="03";
                                }
                                else if(List_input.at(2)=="08"){
                                    BtnS="04";
                                }

                                FilterRFbtn( RF_SAddress, BtnS);

                           }else{
                          print_data("tidak sesuai syarat if \r \n");
                           }
                       }else{
                        print_data("list input tidak sesuai format data \r \n");
                       }
                     }
                }
                else{
                    //qDebug()<<"datain tidak mulai dari DAT";
                }

             return datain;
             }
void MainWindow::cek_MemoriLastButton(QString RF_address,QString btn){
//    ui->table_lastButton->setRowCount(rows);
    bool searching=false;
    RFmemoriNow=RF_address;
    rows=ui->table_lastButton->rowCount();
    qDebug()<<RFmemoriNow+"  "+RFmemoriLast<<endl;

    QDateTime lastbtn= QDateTime::currentDateTime();




    for(int i=0;i<rows;++i){
        if(ui->table_lastButton->item(i,0)->text()==RF_address){

   QString now_string = lastbtn.toString("yyyy-MM-dd HH:mm:ss");

   QTableWidgetItem *T_Button= new QTableWidgetItem(tr("%1").arg(btn));
   ui->table_lastButton->setItem(i,1,T_Button);
   QTableWidgetItem *T_Datetime= new QTableWidgetItem(tr("%1").arg(now_string));
   ui->table_lastButton->setItem(i,2,T_Datetime);
   searching=true;
   break;
       }
    }
    if(!searching){

        QString now_string = lastbtn.toString("yyyy-MM-dd HH:mm:ss");
        const int rowAdder = ui->table_lastButton->rowCount();
        ui->table_lastButton->insertRow(rowAdder);
        QTableWidgetItem *T_RFAddress= new QTableWidgetItem(tr("%1").arg(RF_address));
        ui->table_lastButton->setItem(rowAdder,0,T_RFAddress);
        QTableWidgetItem *T_Button= new QTableWidgetItem(tr("%1").arg(btn));
        ui->table_lastButton->setItem(rowAdder,1,T_Button);
        QTableWidgetItem *T_Datetime= new QTableWidgetItem(tr("%1").arg(now_string));
        ui->table_lastButton->setItem(rowAdder,2,T_Datetime);
        RFmemoriLast=RFmemoriNow;
    }


}

void MainWindow::FilterRFbtn( QString RF_Address, QString btn){
    RFnow=RF_Address;
    btnNow=btn;
     qDebug()<<"btn"<<btnNow<<" "<<btnLast;
    if(startRFbtn==0){
    RFlast=RF_Address;
    btnLast=btn;
    startRFbtn=1;
    }
//    print_data("RF btn now: "+RFnow.toLatin1()+"  "+btnNow.toLatin1()+"\r\n");
//    print_data("RF btn last : "+RFlast.toLatin1()+"  "+btnLast.toLatin1()+"\r\n");

    if((RFnow!=RFlast) && (btnNow!=btnLast) && (btnNow!="02") && (btnLast!="02")){
      time_now=QTime::currentTime();
//       print_data(datain+"\r\n");
       cek_RFaddress(RFnow,btnNow);
       //print_data("a\r\n");
       RFlast=RFnow;

       btnLast=btnNow;
       }
     else if((RFnow!=RFlast)&&(btnNow==btnLast)&&(btnNow!="02") && (btnLast!="02")){
       time_now=QTime::currentTime();
//       print_data(datain+"\r\n");
       cek_RFaddress(RFnow,btnNow);
       //print_data("b\r\n");
       RFlast=RFnow;
       btnLast=btnNow;
       }

     else if(btnNow=="02"){
//        waktu_mulai=QTime::currentTime();
//       print_data(datain+"\r\n");
       //print_data("c\r\n");
       cek_RFaddress(RFnow,btnNow);
       RFlast=RFnow;//
       //btnLast=btnNow;
       }


   else if((RFnow==RFlast)&&(btnNow==btnLast)&&(btnNow!="02") && (btnLast!="02")){
       waktu_mulai= QTime::currentTime();
       if((time_now.secsTo(waktu_mulai))>2){
//         print_data(datain+"\r\n");
         //print_data("d\r\n");
         cek_RFaddress(RFnow,btnNow);
         time_now=QTime::currentTime();
         }
       else{
          print_data("Tekan Tombol Terlalu Cepat\r\n");
         }
   }

  else if((RFnow==RFlast)&&(btnNow!=btnLast)&&(btnNow!="02") && (btnLast!="02")){
       time_now=QTime::currentTime();
//       print_data(datain+"\r\n");
       //print_data("e\r\n");
       cek_RFaddress(RFnow,btnNow);
       RFlast=RFnow;
       btnLast=btnNow;
    }

 }

void MainWindow::insertDB(QString RF_Address,QString Button_ID){
    QSqlQuery query(db);
    QString bfr_btn;
      //masukan serial ke database

     query.prepare(tr("select status_id from t_data_andon where call_id=%1 and rf_address='%2'order by id desc limit 1 ").arg(Button_ID).arg(RF_Address));
     query.exec();
     while(query.next()){
        bfr_btn=query.value(0).toString();
        qDebug()<<"bfr"<<bfr;

     }
    if(bfr_btn=='1'){
        query.prepare(tr("update t_data_andon set time_arrival=current_timestamp, status_id=2 where call_id=%1 and rf_address='%2'").arg(Button_ID).arg(RF_Address));

     }
     else if(bfr_btn=='2' ){
        query.prepare(tr("update t_data_andon set time_finish=current_timestamp, status_id=3 where call_id=%1 and rf_address='%2'").arg(Button_ID).arg(RF_Address));

     }
     else if(bfr_btn=='3'){
         query.prepare ("update t_data_andon set time_finish=current_timestamp");
         query.prepare(tr("insert into t_data_andon (status_id,rf_address,call_id,datetime,plant_id,line_id,process_id,zone_id,subzone_id) values (1,'%1','%2',current_timestamp,'%3','%4','%5','%6','%7')").arg(RF_Address).arg(Button_ID).arg(plant).arg(line).arg(process).arg(zone).arg(subzone));
     }
    else{
     qDebug()<<"bfr null";
     query.prepare(tr("insert into t_data_andon (status_id,rf_address,call_id,datetime,plant_id,line_id,process_id,zone_id,subzone_id) values (1,'%1','%2',current_timestamp,'%3','%4','%5',%6,%7)").arg(RF_Address).arg(Button_ID).arg(plant).arg(line).arg(process).arg(zone).arg(subzone));
      }
      query.exec();
}

void MainWindow::cek_RFaddress(QString RF_Address,QString Button_ID){
    QSqlQuery query(db);
    QString cekRF;

    query.prepare(tr("select rf_address from sma_itx_m_device_list where rf_address like '%1'").arg(RF_Address));
    query.exec();
    while (query.next()){
      cekRF=query.value(0).toString();

    }

    if(cekRF==""){
        print_data("RF Address tidak terdaftar\r\n");
        ui->line_add_RFaddress->setText(RF_Address);
//        ui->checkBox_AddRFAddress->setCheckable(true);
//     if(ui->checkBox_AddRFAddress->isChecked()){
//           add_RFaddress(RF_Address,Button_ID);
//        }
    }

     if(cekRF==RF_Address){
//       ui->checkBox_AddRFAddress->setCheckable(false);
       insertDB(RF_Address,Button_ID);
        cek_MemoriLastButton(RF_Address,Button_ID);
        notifikasi_panggilan(RF_Address,Button_ID);
    }

 }

void MainWindow::on_btn_add_RFaddress_clicked()
{
    QString RFaddress;
    if((ui->line_password_add_RFaddress->text()=="intx123")&&(ui->line_add_RFaddress->text()!="")){
        RFaddress=ui->line_add_RFaddress->text();
        add_RFaddress(RFaddress);
        print_data("RF Address Berhasil di tambahkan!\r\n");


    }else{
         print_data("Cek Kembali RF Address dan password!\r\n");
    }
}

void MainWindow::add_RFaddress(QString RF_Address){

    QSqlQuery query(db);
//sma_itx_m_device_list_seq.nextval
    if(Db==2){
        query.prepare(tr("insert into sma_itx_m_device_list (machine_id,id,rf_address,plant_id,line_id,process_id,zone_id,subzone_id,add_date) values(1,sma_itx_m_device_list_seq.nextval,%1,'1','1','1',1,1,current_timestamp)").arg(RF_Address));

    }else{
        query.prepare(tr("insert into sma_itx_m_device_list (machine_id,rf_address,plant_id,line_id,process_id,zone_id,subzone_id,add_date) values('1',%1,'1','1','1','1','1',current_timestamp)").arg(RF_Address));

    }
    query.exec();
    isi_comboBoxGateway();
    initTable_Simulasi();
}

void MainWindow::autorunAps(){
    Auto_Connect();

    panggil_parameter();
    open_DB();
    initTable_Repeater();
    initTable_Simulasi();
    isi_comboBoxGateway();
    connectUtama();




}
void MainWindow::ambil_nilaiCom(){

   COM=ui->combo_COM->currentText();
    Baudrate=ui->combo_Baudrate->currentText();
     pb_Com= ui->btn_Connect_Comunication->text();

}

void MainWindow::connect_COM()
{
    serial=new QSerialPort(this);
    serial->setPortName(COM);
    if(Baudrate=="9600"){
         serial->setBaudRate(QSerialPort::Baud9600);
         serial->setDataBits(QSerialPort::Data8);
         serial->setParity(QSerialPort::NoParity);
         serial->setStopBits(QSerialPort::OneStop);
         serial->setFlowControl(QSerialPort::NoFlowControl);
    }
    else if(Baudrate=="115200"){
         serial->setBaudRate(QSerialPort::Baud115200);
         serial->setDataBits(QSerialPort::Data8);
         serial->setParity(QSerialPort::NoParity);
         serial->setStopBits(QSerialPort::OneStop);
         serial->setFlowControl(QSerialPort::NoFlowControl);
    }
    if (serial->open(QIODevice::ReadWrite)) {
        qDebug()<<"connect";
        ui->combo_COM->setEnabled(0);
        ui->combo_Baudrate->setEnabled(0);
        ui->btn_Refresh_COM->setEnabled(0);
        ui->btn_Connect_Comunication->setText("Disconnect");
        pb_Com=ui->btn_Connect_Comunication->text();
        print_data("Connect Serial  \r\n");
        connect(serial,static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),this,&MainWindow::cekCOM);
    }
    else
    {
        qDebug()<<"error di serial : "<<serial->errorString();
        print_data("error di serial \r\n");
        }
    }

void MainWindow::on_btn_Refresh_COM_clicked()
{
    ui->combo_COM->setEnabled(1);
    ui->combo_Baudrate->setEnabled(1);
    ui->combo_COM->clear();
    ui->combo_Baudrate->clear();

   Auto_Connect();
}

void MainWindow::Auto_Connect(){
    QList<QSerialPortInfo> com_ports = QSerialPortInfo::availablePorts();
    QSerialPortInfo port;

 foreach(port, com_ports)
 {
         ui->combo_COM->addItem(port.portName());
 }
    ui->combo_Baudrate->addItem("9600");
    ui->combo_Baudrate->addItem("115200");
   COM=ui->combo_COM->currentText();
   Baudrate=ui->combo_Baudrate->currentText();


   }

void MainWindow::cekCOM(QSerialPort::SerialPortError error){
    if(error==QSerialPort::ResourceError){
        print_data("Not Connect Serial  \r\n");
        ui->btn_Connect_Comunication->setText("Connect");

        ui->combo_COM->setEnabled(1);
        ui->combo_Baudrate->setEnabled(1);
       ui->btn_Refresh_COM->setEnabled(1);
        serial->close();
        QThread::msleep(3000);
        on_btn_Connect_Comunication_clicked();
        }
//        QString COMnow;



//        COMnow=

//     for (int i=0;i<com_ports.size();i++) {
//         if (if ){
//              print_data("Not Connect Serial  \r\n");
//              ui->combo_COM->setEnabled(1);
//              ui->combo_Baudrate->setEnabled(1);
//              ui->btn_Refresh_COM->setEnabled(1);
//              ui->btn_Connect_Comunication->setText("Connect");
//         }
//         COMnow=ui->combo_COM->currentIndex(i);
//         qDebug()<<"COM"<<COMnow;
//     }

}


void MainWindow::on_btn_Connect_Comunication_clicked()
{
   connectUtama();
}
void MainWindow::connectUtama(){
    ambil_nilaiCom();
     if(pb_Com=="Connect"){
         connect_COM();

     }
     else if (pb_Com=="Disconnect"){

         ui->combo_COM->setEnabled(1);
         ui->combo_Baudrate->setEnabled(1);
         ui->btn_Connect_Comunication->setText("Connect");
         pb_Com=ui->btn_Connect_Comunication->text();
          ui->btn_Refresh_COM->setEnabled(1);
         serial->close();
     }
     Save_Communication_ini();
}

void MainWindow::disconnected_COM(){

QSerialPort::SerialPortError error;
    if (error == QSerialPort::ResourceError){
         qDebug() << "disconnected";
         serial->close();
    }
}
void MainWindow::print_data(const QByteArray &data){

    QDateTime dateTime = dateTime.currentDateTime();
    QString dateTimeString = dateTime.toString("hh:mm:ss");
    QThread::usleep(10);
    QString time=(tr("%1: ").arg(dateTimeString));
    QThread::usleep(10);
    console->putData(time.toUtf8()+data);
    QThread::usleep(10);

}

void MainWindow::MysqlDB(){
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(Server_DB);
    db.setDatabaseName(nama_DB);
    db.setUserName(Username_DB);
    db.setPassword(Password_DB);

    if(!db.open()){
        QByteArray error = db.lastError().text().toUtf8();
        print_data(error+"\r\n");
         ui->notif_Database->setText("Disconnected");

    }
   else{
    ui->notif_Database->setText("Connected");
    //print_data("Database Mysql\r\n");
    notifDB=1;
   }
}

void MainWindow::SqlserverDB(){
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC3");
    QString Conn = QString("DRIVER={SQL Server};Server=%1;Database=%2;Uid=%3;Port=80;Pwd=%4;WSID=.").arg(Server_DB).arg(nama_DB).arg(Username_DB).arg(Password_DB);
     db.setDatabaseName(Conn);
     if(!db.open()){
         ui->notif_Database->setText("Disconnect");
         QByteArray error = db.lastError().text().toUtf8();
         print_data(error+"\r\n");
         notifDB=4;
     }
     else{
          ui->notif_Database->setText("Connected");
          notifDB=2;
     }
}

void MainWindow::OracleDB(){
    qDebug()<<QSqlDatabase::drivers();
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC3");
    QString Conn;
    if(ui->checkBoxSID->isChecked()){
        print_data("Connect SID\r\n");
        Conn = tr("Driver={Microsoft ODBC for Oracle};Server=(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)(HOST=%1)(PORT=1521))(CONNECT_DATA=(SID=%2)));Uid=%3;Pwd=%4;").arg(Server_DB).arg(nama_DB).arg(Username_DB).arg(Password_DB);
        notifDB=4;
    }
    else{

        print_data("Connect Service Name\r\n");
        Conn = tr("Driver={Microsoft ODBC for Oracle};Server=%1:1521;Uid=%2;Pwd=%3").arg(Server_DB).arg(Username_DB).arg(Password_DB);
       // QSqlDatabase db = QSqlDatabase::addDatabase("QOCI");
//            db.setHostName(Server_DB);
//            db.setDatabaseName(nama_DB);
//            db.setUserName(Username_DB);
//            db.setPassword(Password_DB);
    }

    QByteArray text;
    text.append(Conn);
    print_data(text+"\r\n");

    db.setConnectOptions();
    qDebug()<<"test\n";
    db.setDatabaseName(Conn);

    if(!db.open()){
        QByteArray error = db.lastError().text().toUtf8();
        print_data(error+"\r\n");
        ui->notif_Database->setText("Disconnect");
        notifDB=4;
    }
    else{
        ui->notif_Database->setText("Connected");
        notifDB=3;
    }
}

void MainWindow::open_DB(){

    if(ui->check_Mysql->isChecked()){
         MysqlDB();
         ui->checkBoxOracle->setEnabled(false);
         ui->checkBoxSQLServer->setEnabled(false);
    }
    else if(ui->checkBoxSQLServer->isChecked()){
         SqlserverDB();
         ui->checkBoxOracle->setEnabled(false);
         ui->check_Mysql->setEnabled(false);

    }
    else if (ui->checkBoxOracle->isChecked()){
         OracleDB();
         tandaOracle = 1;
         ui->check_Mysql->setEnabled(false);
         ui->checkBoxSQLServer->setEnabled(false);
    }else{
        ui->check_Mysql->setEnabled(true);
        ui->checkBoxSQLServer->setEnabled(true);
        ui->checkBoxOracle->setEnabled(true);
    }
}

void MainWindow::Connect_TCP(const QString &DATA){
    socket = new QTcpSocket(this);
    socket->connectToHost(ui->line_IP->text(),ui->line_Port->text().toInt());
      if(!socket->waitForConnected(3000)){
        print_data("TCP not Connected\r\n");
        print_data(socket->errorString().toUtf8());
    }
    else {
        QByteArray ba;
        ba.append(DATA);
        socket->write(ba);
        socket->waitForBytesWritten(1000);
        socket->waitForReadyRead(3000);
      }

    QByteArray socketread;
    socketread.append(socket->readAll());

    while(socket->waitForReadyRead(500)){
    socketread.append(socket->readAll());
    qDebug()<<socketread;
    }
    int i=0;
    for(i=0;i<512;i++){
        if(socketread[i]=='"'){
            break;
           }
     }
     socketread.remove(0,i-1);
     print_data(socketread+"\r\n");
}


void MainWindow::on_btn_Submit_clicked()
{

}

void MainWindow::panggil_parameter(){
    QString Loc;
    QString TipeDB_STR,Servername_STR,Username_STR,Password_STR,DBName_STR,Select_RepeaterSTR,SID_STR;//database
    QString IP_STR,Port_STR; //TCP
    QString PLANT_STR, LINE_STR, PROCESS_STR, ZONE_STR, SUBZONE_STR;//Gateway
     QString COM_STR,Baudrate_STR; //Communication



    Loc = qApp->applicationDirPath();
    qDebug()<<"load Configuration";
    QString Loc_Setting = (tr("%1/%2").arg(Loc).arg("Database.ini"));

    QFile setting_ip(Loc_Setting);

    if(setting_ip.open(QIODevice::ReadWrite)){
       QTextStream stream(&setting_ip);
       QString text = stream.readAll();
          qDebug() << text;
          //Parsing data Setting
                  int a=0;
                  int x=0;

                  for(x=0;x<128;x++){
                      if(text[x]=='#'){
                          break;
                      }
                      TipeDB_STR[a++]=text[x];
                  }a=0;
                  for(x+=1;x<128;x++){
                      if(text[x]=='#'){
                          break;
                      }
                      Servername_STR[a++]=text[x];
                  }
                  a=0;
                  for (x+=1;x<128;x++) {
                      if(text[x]=='#'){
                          break;
                      }
                      Username_STR[a++]=text[x];
                  }
                  a=0;
                  for (x+=1;x<128;x++) {
                      if(text[x]=='#'){
                          break;
                      }
                      Password_STR[a++]=text[x];
                  }
                  a=0;
                  for(x+=1;x<128;x++){
                      if(text[x]=='#'){
                          break;
                      }
                      DBName_STR[a++]=text[x];
                  }
                  a=0;
                  for(x+=1;x<128;x++){
                      if(text[x]=='#'){
                          break;
                      }
                      Select_RepeaterSTR[a++]=text[x];
                   }
                  a=0;
                  for(x+=1;x<128;x++){
                      if(text[x]=='#'){
                          break;
                      }
                      SID_STR[a++]=text[x];
                   }
        qDebug()<<Servername_STR<<"#"<<Username_STR<<"#"<<Password_STR<<"#"<<DBName_STR<<"#"<<Select_RepeaterSTR<<"#"<<SID_STR<<"#"<<endl;
}
        Server_DB=Servername_STR;
        Username_DB=Username_STR;
        Password_DB=Password_STR;
        nama_DB=DBName_STR;
        Db=TipeDB_STR.toInt();
        if(SID_STR=="1"){
            ui->checkBoxSID->setChecked(true);
        }else if (SID_STR=="0"){
            ui->checkBoxSID->setChecked(false);
        }

        ui->line_Servername->setText(Server_DB);
        ui->line_Username->setText(Username_DB);
        ui->line_Password->setText(Password_DB);
        ui->line_DBName->setText(nama_DB);
        select_DB();
        ui->line_who_select->setText(Select_RepeaterSTR);

        qDebug()<<"TCP load setting";
        QString Loc_setting2=(tr("%1/%2").arg(Loc).arg("Tcp.ini"));
        QFile setting_ip2(Loc_setting2);

        if(setting_ip2.open(QIODevice::ReadWrite)){
            QTextStream stream(&setting_ip2);
            QString text = stream.readAll();

            qDebug()<<text;

            int a=0;
            int x=0;

            for (x=0;x<128;x++) {
                if(text[x]=='#'){
                    break;
                } IP_STR[a++]=text[x];
            }
            a=0;
            for (x+=1;x<128;x++) {
                if(text[x]=='#'){
                    break;

                }Port_STR[a++]=text[x];
            }
        }
        qDebug()<<"ip:"<<IP_STR<<"port:"<<Port_STR;
        ui->line_IP->setText(IP_STR);
        ui->line_Port->setText(Port_STR);

        qDebug()<<"Gateway setting";

        QString Loc_setting3=(tr("%1/%2").arg(Loc).arg("Gateway.ini"));
        QFile setting_ip3(Loc_setting3);

        if(setting_ip3.open(QIODevice::ReadWrite)){
            QTextStream stream(&setting_ip3);
            QString text = stream.readAll();

            qDebug()<<text;

            int a=0;
            int x=0;

            for (x=0;x<128;x++) {
                if(text[x]=='#'){
                    break;
                } PLANT_STR[a++]=text[x];
            }
            a=0;
            for (x+=1;x<128;x++) {
                if(text[x]=='#'){
                    break;

                }LINE_STR[a++]=text[x];
            }
            a=0;
            for (x+=1;x<128;x++) {
                if(text[x]=='#'){
                    break;

                }PROCESS_STR[a++]=text[x];
            }
            a=0;
            for (x+=1;x<128;x++) {
                if(text[x]=='#'){
                    break;

                }ZONE_STR[a++]=text[x];
            }
            a=0;
            for (x+=1;x<128;x++) {
                if(text[x]=='#'){
                    break;

                }SUBZONE_STR[a++]=text[x];
            }
        }

        plant=PLANT_STR;
        line=LINE_STR;
        process=PROCESS_STR;
        zone=ZONE_STR;
        subzone=SUBZONE_STR;

        ui->line_Plant->setText(PLANT_STR);
        ui->line_Line->setText(LINE_STR);
        ui->line_Process->setText(PROCESS_STR);
        ui->line_Zone->setText(ZONE_STR);
        ui->line_SubZone->setText(SUBZONE_STR);

        qDebug()<<"Communication load setting";
        QString Loc_setting4=(tr("%1/%2").arg(Loc).arg("Communication.ini"));
        QFile setting_ip4(Loc_setting4);

        if(setting_ip4.open(QIODevice::ReadWrite)){
            QTextStream stream(&setting_ip4);
            QString text = stream.readAll();

            qDebug()<<text;

            int a=0;
            int x=0;

            for (x=0;x<128;x++) {
                if(text[x]=='#'){
                    break;
                } COM_STR[a++]=text[x];
            }
            a=0;
            for (x+=1;x<128;x++) {
                if(text[x]=='#'){
                    break;

                }Baudrate_STR[a++]=text[x];
            }
        }
        qDebug()<<"ip:"<<COM_STR<<"port:"<<Baudrate_STR;
        ui->combo_COM->setCurrentText(COM_STR);
        ui->combo_Baudrate->setCurrentText(Baudrate_STR);
        COM=COM_STR;
        Baudrate=Baudrate_STR;

}

void MainWindow::on_btn_Save_Database_clicked()
{
    Save_Database_ini();
    open_DB();
    initTable_Repeater();
    initTable_Simulasi();
    //notif DB di console
    if(notifDB==1){
        print_data("Database Mysql\r\n");
    }
    else if(notifDB==2){
        print_data("Database SqlServer\r\n");
    }
    else if(notifDB==3){
        print_data("Database Oracle\r\n");
    }
    else if(notifDB==4){
         print_data("Database Not Connected\r\n");
    }

    isi_comboBoxGateway();
}

void MainWindow::select_DB(){
    qDebug()<<"nyampecuy"<<Db;
    if(Db==1){
        ui->check_Mysql->setChecked(true);
    }
    if(Db==2){
        ui->checkBoxOracle->setChecked(true);
    }
    if (Db==3){
        ui->checkBoxProcess->setChecked(true);
    }
}

void MainWindow::on_btn_TestTcp_clicked()
{
    socket = new QTcpSocket(this);
    socket->connectToHost(ui->line_IP->text(),ui->line_Port->text().toInt());
      if(!socket->waitForConnected(3000)){
        print_data("TCP not Connected\r\n");
    }
    else {
        print_data("TCP Connected\r\n");
    }
}

void MainWindow::on_btn_SaveTcp_clicked()
{
    Save_Tcp_ini();
    print_data("TCP Saved\r\n");
//    Send_TCP();
}

void MainWindow::Send_TCP(const QString &DATA)
{
    socket = new QTcpSocket(this);
    socket->connectToHost(ui->line_IP->text(),ui->line_Port->text().toInt());
      if(!socket->waitForConnected(3000)){
        qDebug()<<"TCP not Connected\r\n";
        qDebug()<<"Socket Error :"<<socket->errorString();
    }
    else {
        QByteArray ba;
        ba.append(DATA);
        socket->write(ba);
        socket->waitForBytesWritten(1000);
        socket->waitForReadyRead(3000);
      }

    QByteArray socketread;
    socketread.append(socket->readAll());

    while(socket->waitForReadyRead(500)){
    socketread.append(socket->readAll());

    }

    int i=0;
    for(i=0;i<512;i++){
        if(socketread[i]=='#'){
            break;
           }
     }
     socketread.remove(0,i-1);

//     socketread.replace("\n","");
     qDebug()<<"Socket Read: "<<socketread<<endl;
     QString bfr_lineScanBarcode = QString(socketread);
// notif tcp di display/gateway    ui->notif_Scan_Barcode->setText(bfr_lineScanBarcode);
}

//void MainWindow::on_using_TCP_clicked()
//{


////        int value = ui->spinBox_cnt->value();

//        QString request ="GET internex_haengsung/v2/production/spk/4 HTTP/1.0\r\n\r\n\r\n" ;

//        qDebug() << request;
//        Send_TCP(request);

//}

void MainWindow::on_btn_SendCDS_clicked()
{
    print_data("SEND NEW ADRESS COORDINATOR/REPEATER\r\n");
    QString ADDH_NEW, ADDL_NEW, ADDH_target, ADDL_target;

    ADDH_NEW = ui->line_ADDH_Send->text();
    ADDL_NEW = ui->line_ADDL_Send->text();
    ADDH_target = ui->line_ADDH_Fom->text();
    ADDL_target = ui->line_ADDL_From->text();


    QString Message = tr("SEND NEW ADDH : %1 & ADDL : %2 TO %3 %4 \r\n").arg(ADDH_NEW).arg(ADDL_NEW).arg(ADDH_target).arg(ADDL_target);
    QByteArray send;
    send.append(Message);
    print_data(send);

    QByteArray in;
    in.append("ADR#");
    in.append(ADDH_NEW);
     in.append("#");
    in.append(ADDL_NEW);
    in.append("#");

    KirimData_keXbee(in,ADDH_target,ADDL_target);

}
void MainWindow::init_dataCDS(){

}

void MainWindow::KirimData_keXbee(const QByteArray &data, QString &ADDH, QString &ADDL){

    console->putData("SEND to ");
    QString dat=(tr("ADDH=%1  ADDL= %2\r\n").arg(ADDH).arg(ADDL));
    QByteArray str=dat.toLatin1();
    const char *c_str1=str.data();
    console->putData(c_str1);
    console->putData("\r\n");

    //ISI BUAT KIRIM VIA XBEE DATA DAN ADDRESS UDH ADA
    QString data1=data;
    QString comment= (tr("%1\r\n").arg(data1));
    QByteArray comment_array= comment.toLatin1();
    const char *c_comment_array = comment_array.data();
    QByteArray in;
    in.append((char) 0x7E); //Delimiter

    //Length
    in.append((char) 0x00); //Length
    char length_data;
    int size_Buff=comment.size();
    length_data = (size_Buff+0x0E);
    in.append(length_data);
    in.append((char) 0x10); //Frame Type
    in.append((char) 0x01); //Frame ID

    //ADDH
    in.append((char) 0x00);

    QByteArray ADDH_1 = ADDH.toLatin1();
    const char *c_ADDH =ADDH_1.data();
    QByteArray H_ADDH = QByteArray::fromHex(c_ADDH);
         for (int i=0;i<3;i++)
         {
            in.append(H_ADDH[i]);
         }
    //ADDL
    QByteArray ADDL_1 = ADDL.toLatin1();
    const char *c_ADDL =ADDL_1.data();
    QByteArray H_ADDL = QByteArray::fromHex(c_ADDL);
        for (int i=0; i<4;i++)
        {
            in.append(H_ADDL[i]);
        }

    //16Bit address
    in.append((char) 0xFF);
    in.append((char) 0xFE);
    ////////////
    in.append((char) 0x00); //Broadcast Radius
    in.append((char) 0x00); //Option
    ////
     in.append(comment);
    ////
    //CRC
    char crc;
    crc=(0x10+0x01+0xFF+0x00+0x00+0xFE);
        for (int i=0;i<size_Buff;i++){
            crc=crc+c_comment_array[i];
        }
        for (int i=0;i<3;i++){
            crc=crc+H_ADDH[i];
        }
        for (int i=0; i<4;i++){
            crc=crc+H_ADDL[i];
        }

    crc=0xFF-(crc);
    in.append(crc);

    serial->write(in);
    serial->flush();
    serial->waitForBytesWritten(500);
}

void MainWindow::initTable_Repeater(){
    ui->table_repeater->setRowCount(0);
    double row;
    QSqlQuery query(db);
    //QDateTime datetime;
    if(Db==2){
    query.prepare(tr("SELECT ID_DATA,PLANT_ID,LINE_ID,PROCESS_ID,REPEATER,PANEL,CURRENT_STATUS,TO_CHAR(LAST_ACTION, 'YYYY-MM-DD HH24:MI:SS') FROM SMA_ITX_M_REPEATER WHERE ID_DATA IN(%1)").arg(ui->line_who_select->text()));
    }
    else{
        query.prepare(tr("SELECT ID_DATA,PLANT_ID,LINE_ID,PROCESS_ID,REPEATER,PANEL,CURRENT_STATUS,LAST_ACTION FROM SMA_ITX_M_REPEATER WHERE ID_DATA IN(%1)").arg(ui->line_who_select->text()));

    }
    query.exec();

    //qDebug()<<query.prepare(tr("SELECT ID_DATA,PLANT_ID,LINE_ID,PROCESS_ID,REPEATER,PANEL,CURRENT_STATUS,TO_CHAR(LAST_ACTION, 'YYYY-MM-DD HH24:MI:SS') FROM SMA_ITX_M_REPEATER WHERE ID_DATA IN(%1)").arg(ui->line_who_select->text()));

    while(query.next()){
        row=ui->table_repeater->rowCount();
        ui->table_repeater->insertRow(row);
        QTableWidgetItem *item1=new QTableWidgetItem(query.value(0).toString());

        ui->table_repeater->setItem(row,4,item1);

        QTableWidgetItem *item2=new QTableWidgetItem(query.value(1).toString());
        ui->table_repeater->setItem(row,5,item2);

        QTableWidgetItem *item3=new QTableWidgetItem(query.value(2).toString());
        ui->table_repeater->setItem(row,6,item3);

        QTableWidgetItem *item4=new QTableWidgetItem(query.value(3).toString());
        ui->table_repeater->setItem(row,7,item4);

        QTableWidgetItem *item5=new QTableWidgetItem(query.value(4).toString());
        ui->table_repeater->setItem(row,0,item5);

        QTableWidgetItem *item6=new QTableWidgetItem(query.value(5).toString());
        ui->table_repeater->setItem(row,1,item6);

        QTableWidgetItem *item7=new QTableWidgetItem(query.value(6).toString());
        ui->table_repeater->setItem(row,2,item7);

        QTableWidgetItem *item8=new QTableWidgetItem(query.value(7).toString());
        ui->table_repeater->setItem(row,3,item8);

        }


}
void MainWindow::initTable_Simulasi(){

    ui->table_simulasi->setRowCount(0);
    double row;
    QSqlQuery query(db);
    query.prepare(tr("SELECT RF_ADDRESS,MACHINE_ID,PLANT_ID,LINE_ID,PROCESS_ID,ZONE_ID,SUBZONE_ID,ID FROM SMA_ITX_M_DEVICE_LIST WHERE PLANT_ID IN(%1) AND  LINE_ID IN(%2) AND PROCESS_ID IN(%3)AND ZONE_ID IN(%4) AND SUBZONE_ID IN (%5)").arg(ui->line_Plant->text()).arg(ui->line_Line->text()).arg(ui->line_Process->text()).arg(ui->line_Zone->text()).arg(ui->line_SubZone->text()));
    query.exec();
    while(query.next()){
        row = ui->table_simulasi->rowCount();
        ui->table_simulasi->insertRow(row);
        QTableWidgetItem *item1= new QTableWidgetItem(query.value(0).toString());
        ui->table_simulasi->setItem(row,0,item1);

        QTableWidgetItem *item2= new QTableWidgetItem(query.value(1).toString());
        ui->table_simulasi->setItem(row,1,item2);

        QTableWidgetItem *item3= new QTableWidgetItem(query.value(2).toString());
        ui->table_simulasi->setItem(row,2,item3);

        QTableWidgetItem *item4= new QTableWidgetItem(query.value(3).toString());
        ui->table_simulasi->setItem(row,3,item4);

        QTableWidgetItem *item5= new QTableWidgetItem(query.value(4).toString());
        ui->table_simulasi->setItem(row,4,item5);

        QTableWidgetItem *item6= new QTableWidgetItem(query.value(5).toString());
        ui->table_simulasi->setItem(row,5,item6);

        QTableWidgetItem *item7= new QTableWidgetItem(query.value(6).toString());
        ui->table_simulasi->setItem(row,6,item7);

        QTableWidgetItem *item8= new QTableWidgetItem(query.value(7).toString());
        ui->table_simulasi->setItem(row,7,item8);

        }

}


void MainWindow::isi_comboBoxGateway(){
    qDebug()<<"combo box gateway";

    QStringList Plant,Line,Process,Zone,SubZone;
    ui->comboBoxLine->clear();
    ui->comboBoxPlant->clear();
    ui->comboBoxProcess->clear();
    ui->comboBoxSubzone->clear();
    ui->comboBoxZone->clear();



    QSqlQuery qry(db);
    QString query;

    query = QString("SELECT PLANT_ID FROM SMA_ITX_M_DEVICE_LIST GROUP BY PLANT_ID");
    qry.prepare(query);
    qry.exec();
    Plant<<"";

    while(qry.next()){
        Plant<<qry.value(0).toString();
    }
    ui->comboBoxPlant->addItems(Plant);

    query = QString("SELECT LINE_ID FROM SMA_ITX_M_DEVICE_LIST GROUP BY LINE_ID");

    qry.prepare(query);
    qry.exec();
    Line<<"";
    while (qry.next()){
        Line<<qry.value(0).toString();
    }
    ui->comboBoxLine->addItems(Line);

    query = QString("SELECT PROCESS_ID FROM SMA_ITX_M_DEVICE_LIST GROUP BY PROCESS_ID");

    qry.prepare(query);
    qry.exec();

    Process<<"";
    while(qry.next()){
        Process<<qry.value(0).toString();
    }
    ui->comboBoxProcess->addItems(Process);

    query = QString("SELECT ZONE_ID FROM SMA_ITX_M_DEVICE_LIST GROUP BY ZONE_ID");

    qry.prepare(query);
    qry.exec();

    Zone<<"";
    while(qry.next()){
        Zone<<qry.value(0).toString();
    }
    ui->comboBoxZone->addItems(Zone);

    query = QString("SELECT SUBZONE_ID FROM SMA_ITX_M_DEVICE_LIST GROUP BY SUBZONE_ID");

    qry.prepare(query);
    qry.exec();

    SubZone<<"";
    while(qry.next()){
        SubZone<<qry.value(0).toString();
    }
    ui->comboBoxSubzone->addItems(SubZone);

//    Plant.removeAll(QString(""));
//    Line.removeAll(QString(""));
//    Process.removeAll(QString(""));
//    Zone.removeAll(QString(""));
//    SubZone.removeAll(QString(""));

}
void MainWindow::Save_Gateway_ini(){

    QString Loc;
    Loc = qApp->applicationDirPath();

    QString Loc_Setting = (tr("%1/%2").arg(Loc).arg("Gateway.ini"));
    QString PLANT_STR, LINE_STR, PROCESS_STR, ZONE_STR, SUBZONE_STR;



    PLANT_STR= ui->line_Plant->text();
    LINE_STR= ui->line_Line->text();
    PROCESS_STR = ui->line_Process->text();
    ZONE_STR = ui->line_Zone->text();
    SUBZONE_STR = ui->line_SubZone->text();

    QFile setting_ip(Loc_Setting);
    if(setting_ip.open(QIODevice::ReadWrite)){
        QTextStream stream(&setting_ip);
        stream <<PLANT_STR<<"#"<<LINE_STR<<"#"<<PROCESS_STR <<"#" <<ZONE_STR<<"#" <<SUBZONE_STR << "#" <<endl;
        }

    setting_ip.close();



}

void MainWindow::Save_Communication_ini(){
    QString Loc;
    Loc=qApp->applicationDirPath();

    QString Loc_Setting2 = (tr("%1/%2").arg(Loc).arg("Communication.ini"));
    QString COM_STR,Baudrate_STR;

    COM_STR=ui->combo_COM->currentText();
    Baudrate_STR= ui->combo_Baudrate->currentText();

    QFile setting_ip2(Loc_Setting2);
    if(setting_ip2.open(QIODevice::ReadWrite)){
        QTextStream stream(&setting_ip2);
        stream<<COM_STR<<"#"<<Baudrate_STR<<"#"<<endl;
    }
}

void MainWindow::Save_Database_ini(){

    QString Tipe_DB,Servername_STR,Username_STR,Password_STR,DBName_STR,Select_Repeater,SID_STR;
    QString Loc;
    Loc=qApp->applicationDirPath();

    QString Loc_Setting = (tr("%1/%2").arg(Loc).arg("Database.ini"));


    if(ui->check_Mysql->isChecked()){
                Tipe_DB="1";
    }
    else if(ui->checkBoxOracle->isChecked()){
        Tipe_DB="2";
    }
    else if (ui->checkBoxSQLServer->isChecked()){
        Tipe_DB="3";
    }
    if(ui->checkBoxSID->isChecked()){
        SID_STR = "1";

    }if(SID_STR==""){
        SID_STR="0";
    }



    Servername_STR = ui->line_Servername->text();
    Username_STR = ui->line_Username->text();
    Password_STR = ui->line_Password->text();
    DBName_STR = ui->line_DBName->text();
    Select_Repeater=ui->line_who_select->text();


    QFile setting_ip(Loc_Setting);
    if(setting_ip.open(QIODevice::ReadWrite)){
        QTextStream stream(&setting_ip);
        stream<<Tipe_DB<<"#"<<Servername_STR<<"#"<<Username_STR<<"#"<<Password_STR<<"#"<<DBName_STR<<"#"<<Select_Repeater<<"#"<<SID_STR<<"#"<<endl;
    }
    setting_ip.close();
}

//{

//    QString Loc;
//    Loc = qApp->applicationDirPath();

//    QString Loc_Setting2 = (tr("%1/%2").arg(Loc).arg("setting.ini"));
//    QString PLANT_STR, LINE_STR, PROCESS_STR, ZONE_STR, SUBZONE_STR;



//    PLANT_STR= ui->line_plant->text();
//    LINE_STR= ui->line_line->text();
//    PROCESS_STR = ui->line_proses->text();
//    ZONE_STR = ui->line_zone->text();
//    SUBZONE_STR = ui->line_subzone->text();

//    QFile setting_ip2(Loc_Setting2);
//    if(setting_ip2.open(QIODevice::ReadWrite)){
//        QTextStream stream(&setting_ip2);
//        stream << PLANT_STR <<"#"<<LINE_STR<<"#"<<PROCESS_STR <<"#" <<ZONE_STR<<"#" <<SUBZONE_STR << "#" <<endl;
//        }

//}
void MainWindow::Save_Tcp_ini(){
    QString Loc;
    Loc=qApp->applicationDirPath();

    QString IP_STR,Port_STR;
    QString Loc_Setting = (tr("%1/%2").arg(Loc).arg("Tcp.ini"));

    IP_STR = ui->line_IP->text();
    Port_STR = ui->line_Port->text();

    QFile setting_ip(Loc_Setting);
    if(setting_ip.open(QIODevice::ReadWrite)){
        QTextStream stream(&setting_ip);
        stream<<IP_STR<<"#"<<Port_STR<<"#"<<endl;

    }setting_ip.close();
}






void MainWindow::on_comboBoxPlant_currentIndexChanged(const QString &arg1)
{
 if (ui->checkBoxPlant->isChecked()){

     if(arg1!=""){
         if(ui->line_Plant->text()==""){
             ui->line_Plant->setText("'"+arg1+"'");
         }
         else{
             ui->line_Plant->setText(ui->line_Plant->text()+","+"'"+arg1+"'");
         }

     }else{
          //ui->line_Plant->setText("");
     }
 }else{
     if(arg1!=""){
         if(ui->line_Plant->text()==""){
             ui->line_Plant->setText(arg1);
         }
         else{
             ui->line_Plant->setText(ui->line_Plant->text()+","+arg1);
         }

     }else{
          //ui->line_Plant->setText(arg1);
     }
 }

}

void MainWindow::on_comboBoxLine_currentIndexChanged(const QString &arg1)
{
    if (ui->checkBoxLine->isChecked()){

        if(arg1!=""){
            if(ui->line_Line->text()==""){
                ui->line_Line->setText("'"+arg1+"'");
            }
            else{
                ui->line_Line->setText(ui->line_Line->text()+","+"'"+arg1+"'");
            }

        }else{
            //ui->line_Line->setText("");
        }
    }else{
        if(arg1!=""){
            if(ui->line_Line->text()==""){
                ui->line_Line->setText(arg1);
            }
            else{
                ui->line_Line->setText(ui->line_Line->text()+","+arg1);
            }

        }else{
            // ui->line_Line->setText(arg1);
        }
    }
}

void MainWindow::on_comboBoxProcess_currentIndexChanged(const QString &arg1)
{
    if (ui->checkBoxProcess->isChecked()){

        if(arg1!=""){
            if(ui->line_Process->text()==""){
                ui->line_Process->setText("'"+arg1+"'");
            }
            else{
                ui->line_Process->setText(ui->line_Process->text()+","+"'"+arg1+"'");
            }

        }else{
            // ui->line_Process->setText("");
        }
    }else{
        if(arg1!=""){
            if(ui->line_Process->text()==""){
                ui->line_Process->setText(arg1);
            }
            else{
                ui->line_Process->setText(ui->line_Process->text()+","+arg1);
            }

        }else{
            // ui->line_Process->setText(arg1);
        }
    }
}

void MainWindow::on_comboBoxZone_currentIndexChanged(const QString &arg1)
{
    if (ui->checkBoxZone->isChecked()){

        if(arg1!=""){
            if(ui->line_Zone->text()==""){
                ui->line_Zone->setText("'"+arg1+"'");
            }
            else{
                ui->line_Zone->setText(ui->line_Zone->text()+","+"'"+arg1+"'");
            }

        }else{
            // ui->line_Zone->setText(arg1);
        }
    }else{
        if(arg1!=""){
            if(ui->line_Zone->text()==""){
                ui->line_Zone->setText(arg1);
            }
            else{
                ui->line_Zone->setText(ui->line_Zone->text()+","+arg1);
            }

        }else{
             //ui->line_Zone->setText(arg1);
        }
    }
}

void MainWindow::on_comboBoxSubzone_currentIndexChanged(const QString &arg1)
{
    if (ui->checkBoxSubZone->isChecked()){

        if(arg1!=""){
            if(ui->line_SubZone->text()==""){
                ui->line_SubZone->setText("'"+arg1+"'");
            }
            else{
                ui->line_SubZone->setText(ui->line_SubZone->text()+","+"'"+arg1+"'");
            }

        }else{
             //ui->line_SubZone->setText("");
        }
    }else{
        if(arg1!=""){
            if(ui->line_SubZone->text()==""){
                ui->line_SubZone->setText(arg1);
            }
            else{
                ui->line_SubZone->setText(ui->line_SubZone->text()+","+arg1);
            }

        }else{
            // ui->line_SubZone->setText(arg1);
        }
    }
}



void MainWindow::on_btn_Submit_Gateway_clicked()
{
    Save_Gateway_ini();
    ui->tabWidget->setCurrentIndex(0);
    initTable_Simulasi();
    print_data("Saved Filter Gateway Data\r\n");
}

void MainWindow::on_Who_button_clicked()
{
    print_data("Who Command \r\n");
    QByteArray in;
        in.append((char) 0x7E); //Delimiter
        in.append((char) 0x00); //Length
        in.append((char) 0x13); //Length
        in.append((char) 0x10); //Frame Type
        in.append((char) 0x01); //Frame ID
        in.append((char) 0x00); //46Bit address
        in.append((char) 0x00);
        in.append((char) 0x00);
        in.append((char) 0x00);
        in.append((char) 0x00);
        in.append((char) 0x00);
        in.append((char) 0xFF);
        in.append((char) 0xFF);
        in.append((char) 0xFF); //16Bit address
        in.append((char) 0xFE);
        in.append((char) 0x00); //Broadcast Radius
        in.append((char) 0x00); //Option
        in.append((char) 0x57);
        in.append((char) 0x48);
        in.append((char) 0x4F);
        in.append((char) 0x0D);
        in.append((char) 0x0A);
        in.append((char) 0xEE); //crc

        serial->write(in);
        serial->flush();
        serial->waitForBytesWritten(500);
        // menghapus isi notif DB

}

void MainWindow::on_DirectWho_button_clicked()
{

    print_data("Dirrect WHO\r\n");
    QByteArray text = "WHO\r\n";
    QString ADDH = "13A200";
    QString ADDL = ui->ADDL_DirectWho->text();
    KirimData_keXbee(text,ADDH,ADDL);
}



void MainWindow::on_btn_Clear_Log_clicked()
{
   console->clear();
}

void MainWindow::on_btn_SearchRF_clicked()
{
    int rows = ui->table_simulasi->rowCount();
    QString nilaiSearch=ui->line_RFAddress->text();
    bool founded = false;
    for(int i = 0; i < rows; ++i)
    {
        if(ui->table_simulasi->item(i, 0)->text() == nilaiSearch)
        {

            founded  = true;
            print_data((tr("ditemukan %1\r\n").arg(i)).toUtf8());
            break;
        }
    }
    if(!founded )
    {

        print_data("tidak Ditemukan\r\n");
    }
}

void MainWindow::on_btn_MachineRF_clicked()
{
     QString nilaiSearch=ui->line_RFAddress->text();
     FilterRFbtn(nilaiSearch,"04");
     print_data("Simulasi Button Machine\r\n");


}

void MainWindow::on_btn_MaterialRF_clicked()
{
     QString nilaiSearch=ui->line_RFAddress->text();
      FilterRFbtn(nilaiSearch,"01");
      print_data("Simulasi Button Material\r\n");
}

void MainWindow::on_btn_QualityRF_clicked()
{
     QString nilaiSearch=ui->line_RFAddress->text();
      FilterRFbtn(nilaiSearch,"08");
      print_data("Simulasi Button Quality\r\n");
}

void MainWindow::on_btn_StopRF_clicked()
{
     QString nilaiSearch=ui->line_RFAddress->text();
      FilterRFbtn(nilaiSearch,"02");
      print_data("Simulasi Button Stop\r\n");
}

void MainWindow::on_actionCommunication_triggered()
{
    ui->Main_Body->setCurrentIndex(0);



}

void MainWindow::on_actionFilter_Gateway_Data_triggered()
{

    ui->Main_Body->setCurrentIndex(1);//gteway

}

void MainWindow::on_actionDatabase_triggered()
{
    ui->Main_Body->setCurrentIndex(2);//database

}
void MainWindow::on_actionTCP_triggered()
{
    ui->Main_Body->setCurrentIndex(3);//tcp

}

void MainWindow::on_actionChange_Destination_Send_triggered()
{

    ui->Main_Body->setCurrentIndex(6);//cds
}

void MainWindow::on_actionRF_Address_Simulasi_triggered()
{
     ui->Main_Body->setCurrentIndex(4);//RFS
     ui->tabWidget->setCurrentIndex(0);
}



//void MainWindow::on_table_simulasi_cellClicked(int row, int column)
//{
//}

void MainWindow::on_line_who_select_editingFinished()
{
    initTable_Repeater();
}




void MainWindow::on_table_simulasi_clicked(const QModelIndex &index)
{
        if(index.isValid()){
            cellText = index.data().toString();
            if(cellText.size()==5){
            ui->line_RFAddress->setText(cellText);
            }
        }
// ui->Main_Body->setCurrentIndex(6);//cds
}

void MainWindow::on_Main_Body_currentChanged(int arg1)
{
    if(arg1==5){
//        ui->Main_Body->addWidget(console);
//        console->setEnabled(true);
    }
}

void MainWindow::on_percobaanRadio_clicked()

{   TableEdit.radiobox_filter(db);
    TableEdit.exec();
}


void MainWindow::on_table_repeater_clicked(const QModelIndex &index)
{

            if(index.isValid()){
                cellText = index.data().toString();
                if(cellText.size()==8){
                ui->ADDL_DirectWho->setText(cellText);
                }
            }


}

void MainWindow::on_actionAdd_Button_triggered()
{
     ui->Main_Body->setCurrentIndex(5);//RFS
}


void MainWindow::on_btn_clear_RFaddress_add_clicked()
{
    ui->line_add_RFaddress->clear();
    ui->line_password_add_RFaddress->clear();
}
