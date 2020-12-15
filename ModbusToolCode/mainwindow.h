#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "commsetdialog.h"
#include <QLabel>
#include <QProgressBar>
#include <QSqlDatabase>
#include <QTranslator>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDateTime>
#include <QThread>
#include <QSqlTableModel>
#include <QStandardItemModel>
#include "serial.h"
#include "chooseenumitemdialog.h"
#include "datatimedialog.h"

struct Signal_Info
{
    uint scene_zone_id;
    uint scene_zone_type;
    QString Display;
    QVariant Value;
    QString Unit;
    QString property;
    float factor;
    uint data_type;
    uint address;
    uint reg;
    QString range;
    uint offset;
};
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void closeEvent(QCloseEvent *) override;
public:
    Serial *serial;
    QThread *thread;
    CommSetInfo commsetinfo;
    static uint addr;
    static uint outtime;
    static QString com;
    static int Baud;
    static QString DbName;
    static QList<uint> scene_items_num_list;  //每个画面的内容数（画面不重复）
    static QList<Signal_Info> Signal_info_list;
    QList<QString> scene_in_use_name;
    QList<uint> scene_in_use_id;
    static Signal_Info Clicked_Signal;
    static QByteArray Write_Value;
    QLabel *error_tip_label;
    QProgressBar *progressbar;
    QSqlDatabase db;
    int curr_progressbar_maxinum = 0;
    void init();
    QVariant Value_Display_Process(Signal_Info s);  //值显示处理函数
    QStringList Signal_Info_Property(QString s);
    QTranslator translator;
private:
    QList<uint> scene_zone_id_list;    //用到的画面的id集合(不重复)
    QList<uint> scene_zone_type_list;
    QList<uint> signal_id_list;
    Signal_Info Read_Signal_info_list(int scene_zone_id,int item_pos);
    Signal_Info Write_Signal_info_list(int scene_zone_id,int item_pos,QVariant v);
    const uint English = 1;
    const uint Chinese = 0;
signals:
    void CloseSignal();
    void Doworks_Signal();
    void Stop_Thread_Signal();
    void Open_Com_Signal();
public slots:
    void Comm_Set_Info_Receive(QVariant& v);
    void DbNameSlot(QString arg);
    void Data_Updata_Slot(int cnt);
    void Error_Tip_Slot(QString str);
    void Property_Choice_Receive(QString s);
    void DataTime_Slot(QDateTime datatime);
private slots:
    void on_CommAction_triggered();

    void on_treeView_clicked(const QModelIndex &index);

    void on_RunAction_triggered();

    void on_StopAction_triggered();

    void on_tableView_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
