#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QTableView>
#include <QDebug>
#include <QMenu>
#include <QInputDialog>
#include <QSqlTableModel>
#include <QFile>
#include <QSqlRecord>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTranslator>
#include <QStandardItemModel>
#include <QSqlQuery>
#include "addsignaldialog.h"
#include "addscenezonedialog.h"
#include "addsceneitemdialog.h"
#include "scenemgrdialog.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void CloseSignal();
    void Send_Modify_Signal_Info(QVariant);
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
public:
    QTranslator translator;
    static QString DbName;
    QSqlDatabase db;
    int row_mouse_catch_index = -1;
    int row_mouse_catch_index1 = -1;
    void closeEvent(QCloseEvent *) override;
    void TableBindDb(QString DB);
    void TableViewInit();
    void Scene_Treeview_Init();
    void DBTableAddLine( QTableView* view);
    void DBTableDelLine(QTableView* view);
    void DBTableModifyLine(QTableView* view);
    QStandardItem* getTopParent(QStandardItem* item);
    QModelIndex getTopParent(QModelIndex itemIndex);
public slots:
    void on_listWidget_currentRowChanged(int currentRow);
    void DbNameSlot(QString arg);
    void Signal_Zone_Menu(const QPoint&);
    void Signal_Zone_Menu_Action1();
    void Signal_Zone_Menu_Action2();
    void Signal_Menu(const QPoint&);
    void Signal_Menu_Action1();
    void Signal_Menu_Action2();
    void Signal_Menu_Action3();
    void Scene_Zone_Menu(const QPoint& );
    void Scene_Zone_Menu_Action1();
    void Scene_Zone_Menu_Action2();
    void Scene_Tree_Menu(const QPoint& );
    void Scene_Tree_Menu_Action1();
    void Scene_Tree_Menu_Action2();
    void Scene_Tree_Menu_Action3();
    void Signal_Add_Info_Receive(QVariant& v);
    void Scene_Zone_Add_Info_Receive(QVariant& v);
    void Scene_Item_Add_Info_Receive(QString& str);
    void Send_Scene_Choose_Info_Receive(QString& str);
    void row_mouse_catch(const QModelIndex & index);

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_SignalZoneView_clicked(const QModelIndex &index);

    void on_AddButton_clicked();

    void on_DelButton_clicked();

    void on_SceneZoneView_clicked(const QModelIndex &index);

    void on_UpButton_clicked();

    void on_DownButton_clicked();

    void on_SceneView_clicked(const QModelIndex &index);

    void on_SaveButton_clicked();

    void on_SyButton_clicked();

    void on_XyButton_clicked();

    void on_UpdateButton_clicked();

    void on_ReleaseButton_clicked();

protected:

    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
private:
    int mousePosX;//记录有效位置的X
    int mousePosY;//记录有效位置的Y
    bool mouseFlagX = false;//记录是否可以进行左右移动
    bool mouseFlagY = false;//记录是否可以开始上下移动
    bool ModifySignalFlag = false;
    const uint English = 1;
    const uint Chinese = 0;

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
