#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "console.h"
#include <QSerialPort>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QtSql>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQueryModel>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QList>
#include<QDateTime>
#include <QThread>
#include <QFile>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSystemTrayIcon>
#include "tableedit.h"
#include "ui_tableedit.h"
#include <QStandardItemModel>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
class QAction;
class QStandardItemModel;
class QString;
class QLineEdit;
class QLabel;
class QPushButton;
class QGridLayout;
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Console;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString Port_TCP,Baudrate,IP_TCP,COM;
    QString pb_Com;
    QString Server_DB,Username_DB,Password_DB,nama_DB;
    QString plant,line,process,zone,subzone;
    int Db;
    QByteArray bfr;
    QTime waktu_mulai,time_now;
    QString RF_SAddress,BtnS;
    QString RFnow,RFlast;
    QString RFmemoriNow,RFmemoriLast;
    QString btnLast,btnNow;
    QString cellText;
    int filter_input;
    int startRFbtn,startmemori;
    int rows;
    short int notifDB;
    int tandaOracle;
    QString bfrRF,bfrMachine, bfrPlant, bfrLine, bfrProcess, bfrZone, bfrSubzone, bfrDevice;
    tableedit TableEdit;
    Ui::tableedit *tabelEdit;


private slots:
    void delete_item_table_simulasi();

    void deleteItem();

    void Send_TCP(const QString &DATA);

    void update_table_simulasi();

    void cekCOM(QSerialPort::SerialPortError error);
//    void update_memori(QString button);
//    void add_memori(QString button, QString RFAddress);

//    void save_nilai_edit_table_simulasi();

    void MysqlDB();

    void SqlserverDB();

    void OracleDB();

    void changeEvent(QEvent* e);

//    void closeEvent(QCloseEvent *event);

    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    void createTrayIcon();

    void filterdataWho(QByteArray dataWho);

    void connectUtama();

    void Insert_Data(QString DataWHO);

    void cek_MemoriLastButton(QString RF_address,QString Btn);

    void FilterRFbtn( QString RF_Address, QString Btn);

    void add_RFaddress(QString RF_Address);

    void cek_RFaddress(QString RF_Address,QString Button_ID);

    void insertDB(QString RF_Address,QString Button_ID);

    QByteArray cleardata(QByteArray datain);

    void processRead();

//    void Insert_Data(QString DataWHO);

    void autorunAps();

    void init_dataCDS();

    void KirimData_keXbee(const QByteArray &data, QString &ADDH, QString &ADDL);

    void select_DB();

    void Save_Tcp_ini();

    void initTable_Repeater();

    void initTable_Simulasi();

    void Save_Communication_ini();

    void Save_Gateway_ini();

    void isi_comboBoxGateway();

    void panggil_parameter();

    void Save_Database_ini();

    void print_data(const QByteArray &data);

    void Auto_Connect();

    void open_DB();

    void ambil_nilaiCom();



    void on_btn_Connect_Comunication_clicked();

    void on_btn_Refresh_COM_clicked();

    void on_btn_Submit_clicked();

    void on_btn_Save_Database_clicked();

    void on_btn_TestTcp_clicked();

    void on_btn_SaveTcp_clicked();

    void on_btn_SendCDS_clicked();

    void connect_COM();

    void disconnected_COM();

    void Connect_TCP(const QString &DATA);

    void on_comboBoxPlant_currentIndexChanged(const QString &arg1);

    void on_comboBoxLine_currentIndexChanged(const QString &arg1);

    void on_comboBoxProcess_currentIndexChanged(const QString &arg1);

    void on_comboBoxZone_currentIndexChanged(const QString &arg1);

    void on_comboBoxSubzone_currentIndexChanged(const QString &arg1);


    void on_btn_Submit_Gateway_clicked();

    void on_Who_button_clicked();

    void on_DirectWho_button_clicked();

    void on_btn_Clear_Log_clicked();

    void on_btn_SearchRF_clicked();

    void on_btn_MachineRF_clicked();

    void on_btn_MaterialRF_clicked();

    void on_btn_QualityRF_clicked();

    void on_btn_StopRF_clicked();

    void createActions();

    void notifikasi_panggilan(QString Address,QString button);

    void on_actionCommunication_triggered();

    void on_actionFilter_Gateway_Data_triggered();

    void on_actionDatabase_triggered();

    void on_actionTCP_triggered();

    void on_actionChange_Destination_Send_triggered();

    void on_actionRF_Address_Simulasi_triggered();

    void on_line_who_select_editingFinished();

    void updateItem();

    void addItem();

    void on_table_simulasi_customContextMenuRequested(const QPoint &pos);

    void on_table_simulasi_clicked(const QModelIndex &index);

    void on_Main_Body_currentChanged(int arg1);

    void on_percobaanRadio_clicked();

    void on_table_repeater_clicked(const QModelIndex &index);

    void on_actionAdd_Button_triggered();

    void on_btn_add_RFaddress_clicked();

    void on_btn_clear_RFaddress_add_clicked();

private:
    //guiadd
    QLineEdit *lineEdit;
    QLabel *label;
    QPushButton *button;
    QGridLayout *layout;

    QMenu *contextmenu;
    QTableWidget *table;
    QStandardItemModel*Model;



    Ui::MainWindow *ui;
    QString DataAsString;
    Console *console;
    QSerialPort *serial;
    QSqlDatabase db;
    QSqlQuery qry;
    QSqlQueryModel *model;
    QTcpSocket *socket;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;


};
#endif // MAINWINDOW_H
