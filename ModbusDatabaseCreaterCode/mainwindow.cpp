#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"

uint LoginDialog::Language;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if(LoginDialog::Language == English)
    {
        translator.load(":/Language/C_I18N_en_UK.qm");
        QApplication::instance()->installTranslator(&translator);
        ui->retranslateUi(this);
    }
    ui->stackedWidget->resize(747,360);  //解决重绘事件bug
    setMouseTracking(true);
    centralWidget()->setMouseTracking(true);
    ui->SignalZoneView->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    ui->SignalView->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    ui->SceneZoneView->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    ui->groupBox_1->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    ui->groupBox_2->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    ui->SceneTreeView->setMouseTracking(true);
    ui->stackedWidget->setMouseTracking(true);
    ui->stackedWidget->widget(0)->setMouseTracking(true);
    ui->stackedWidget->widget(1)->setMouseTracking(true);
    ui->stackedWidget->widget(2)->setMouseTracking(true);
    ui->stackedWidget->widget(3)->setMouseTracking(true);
    ui->stackedWidget->widget(4)->setMouseTracking(true);
    TableViewInit();
    ui->SignalZoneView->setContextMenuPolicy(Qt::CustomContextMenu);        //右击创建菜单
    ui->SignalView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->SceneZoneView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->SceneTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->SignalZoneView, SIGNAL(customContextMenuRequested(const QPoint&)),
                this, SLOT(Signal_Zone_Menu(const QPoint& )));
    connect(ui->SignalView, SIGNAL(customContextMenuRequested(const QPoint&)),
                this, SLOT(Signal_Menu(const QPoint& )));
    connect(ui->SceneZoneView, SIGNAL(customContextMenuRequested(const QPoint&)),
                this, SLOT(Scene_Zone_Menu(const QPoint& )));
    connect(ui->SceneTreeView, SIGNAL(customContextMenuRequested(const QPoint&)),
                this, SLOT(Scene_Tree_Menu(const QPoint& )));

    connect(ui->SceneTreeView,&QTreeView::entered,this,&MainWindow::row_mouse_catch);   //鼠标行捕捉


}

MainWindow::~MainWindow()
{
   delete ui;
}

void MainWindow::closeEvent(QCloseEvent *)
{

    emit CloseSignal();
    this->deleteLater();
}


void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}

void MainWindow::DbNameSlot(QString arg)
{
    ui->label_3->setText(arg);
    ui->label_4->setText("./Config/"+arg+".db");
    DbName = arg;
    TableBindDb(DbName);
}

void MainWindow::TableBindDb(QString DB)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./Config/"+DB+".db");
    if(QFile::exists("./Config/"+DB+".db"))
    {
        if(db.open())
        {
            QSqlTableModel *pModel = new QSqlTableModel(this,db);
            pModel->setTable("users");
            // 三种提交方式，改动即提交，选择其他行时提交，手动提交；经实际测试，其中只有手动提交在显示效果上是最好的
            pModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
            pModel->select();
            pModel->setHeaderData(0, Qt::Horizontal, "user");
            pModel->setHeaderData(1, Qt::Horizontal, "password");
            pModel->sort(0, Qt::AscendingOrder);
            ui->UserView->setModel(pModel);

            QSqlTableModel *pModel1 = new QSqlTableModel(this,db);
            pModel1->setTable("signal_zone");
            pModel1->setEditStrategy(QSqlTableModel::OnManualSubmit);
            pModel1->select();
            pModel1->setHeaderData(1, Qt::Horizontal, tr("信号分组"));
            pModel1->sort(0, Qt::AscendingOrder);
            ui->SignalZoneView->setModel(pModel1);
            ui->SignalZoneView->hideColumn(0);  //隐藏第一列

            QSqlTableModel *pModel2 = new QSqlTableModel(this,db);
            pModel2->setTable("scene_zone");
            pModel2->setEditStrategy(QSqlTableModel::OnManualSubmit);
            pModel2->select();
            pModel2->setHeaderData(3, Qt::Horizontal, tr("画面分组"));
            pModel2->sort(0, Qt::AscendingOrder);
            ui->SceneZoneView->setModel(pModel2);
            ui->SceneZoneView->hideColumn(0);  //隐藏前三列
            ui->SceneZoneView->hideColumn(1);
            ui->SceneZoneView->hideColumn(2);

            QSqlTableModel *pModel3 = new QSqlTableModel(this,db);
            pModel3->setTable("signal");
            pModel3->setEditStrategy(QSqlTableModel::OnManualSubmit);
            pModel3->select();
            pModel3->sort(0, Qt::AscendingOrder);
            ui->SignalView->setModel(pModel3);

            QSqlTableModel *pModel4 = new QSqlTableModel(this,db);
            pModel4->setTable("scene_items");
            pModel4->setEditStrategy(QSqlTableModel::OnManualSubmit);
            pModel4->select();
            pModel4->sort(0, Qt::AscendingOrder);
            ui->SceneView->setModel(pModel4);

            QSqlTableModel *pModel5 = new QSqlTableModel(this,db);
            pModel5->setTable("signal");
            pModel5->setEditStrategy(QSqlTableModel::OnManualSubmit);
            pModel5->select();
            pModel5->sort(0, Qt::AscendingOrder);
            ui->Scene_Signal_Info_View->setModel(pModel5);

            QSqlTableModel *pModel6 = new QSqlTableModel(this,db);
            pModel6->setTable("translation");
            pModel6->setEditStrategy(QSqlTableModel::OnManualSubmit);
            pModel6->select();
            pModel6->sort(0, Qt::AscendingOrder);
            ui->TranslationView->setModel(pModel6);

            QStandardItemModel *model = new QStandardItemModel();
            model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("Service"));
            ui->SceneTreeView->setModel(model);

            Scene_Treeview_Init();

        }

    }
}

void MainWindow::TableViewInit()   //表格初始化
{
    // 只能单选
    ui->UserView->setSelectionMode(QAbstractItemView::SingleSelection);
    //重新调整列以自适应内容的宽度
    ui->UserView->resizeColumnsToContents();
    //排序使能
    ui->UserView->setSortingEnabled(true);
    //在水平头显示排序按钮
    ui->UserView->horizontalHeader()->sortIndicatorOrder();
    //隐藏第n列，一般可用于连接数据库后隐藏主键所在的列
    //ui->UserView->hideColumn(0);
    // 以行作为选择标准
    ui->UserView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 行头隐藏
    ui->UserView->verticalHeader()->hide();
    // 让列头可被点击，触发点击事件
    ui->UserView->horizontalHeader()->setSectionsClickable(true);
    // 去掉选中表格时，列头的文字高亮
    ui->UserView->horizontalHeader()->setHighlightSections(false);
    ui->UserView->horizontalHeader()->setBackgroundRole(QPalette::Background);
    // 列头灰色
   // ui->UserView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(225,225,225)};");


    ui->SignalZoneView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->SignalZoneView->resizeColumnsToContents();
    ui->SignalZoneView->setSortingEnabled(true);
    ui->SignalZoneView->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    ui->SignalZoneView->horizontalHeader()->sortIndicatorOrder();
    ui->SignalZoneView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->SignalZoneView->verticalHeader()->hide();
    ui->SignalZoneView->horizontalHeader()->setSectionsClickable(true);
    ui->SignalZoneView->horizontalHeader()->setHighlightSections(false);
    ui->SignalZoneView->horizontalHeader()->setBackgroundRole(QPalette::Background);


    ui->SceneZoneView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->SceneZoneView->resizeColumnsToContents();
    ui->SceneZoneView->setSortingEnabled(true);
    ui->SceneZoneView->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    ui->SceneZoneView->horizontalHeader()->sortIndicatorOrder();
    ui->SceneZoneView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->SceneZoneView->verticalHeader()->hide();
    ui->SceneZoneView->horizontalHeader()->setSectionsClickable(true);
    ui->SceneZoneView->horizontalHeader()->setHighlightSections(false);
    ui->SceneZoneView->horizontalHeader()->setBackgroundRole(QPalette::Background);

    ui->SignalView->setSelectionMode(QAbstractItemView::SingleSelection);
    //ui->SignalView->resizeColumnsToContents();
    ui->SignalView->setSortingEnabled(true);
    ui->SignalView->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    ui->SignalView->horizontalHeader()->sortIndicatorOrder();
    ui->SignalView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->SignalView->verticalHeader()->hide();
    ui->SignalView->horizontalHeader()->setSectionsClickable(true);
    ui->SignalView->horizontalHeader()->setHighlightSections(false);
    ui->SignalView->horizontalHeader()->setBackgroundRole(QPalette::Background);

    ui->SceneView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->SceneView->setSortingEnabled(true);
    //ui->SceneView->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    ui->SceneView->horizontalHeader()->sortIndicatorOrder();
    ui->SceneView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->SceneView->verticalHeader()->hide();
    ui->SceneView->horizontalHeader()->setSectionsClickable(true);
    ui->SceneView->horizontalHeader()->setHighlightSections(false);
    ui->SceneView->horizontalHeader()->setBackgroundRole(QPalette::Background);

    ui->Scene_Signal_Info_View->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->Scene_Signal_Info_View->setSortingEnabled(true);
    ui->Scene_Signal_Info_View->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    ui->Scene_Signal_Info_View->horizontalHeader()->sortIndicatorOrder();
    ui->Scene_Signal_Info_View->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->Scene_Signal_Info_View->verticalHeader()->hide();
    ui->Scene_Signal_Info_View->horizontalHeader()->setSectionsClickable(true);
    ui->Scene_Signal_Info_View->horizontalHeader()->setHighlightSections(false);
    ui->Scene_Signal_Info_View->horizontalHeader()->setBackgroundRole(QPalette::Background);

    ui->TranslationView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->TranslationView->setSortingEnabled(true);
    ui->TranslationView->horizontalHeader()->sortIndicatorOrder();
    ui->TranslationView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->TranslationView->horizontalHeader()->setSectionsClickable(true);
    ui->TranslationView->horizontalHeader()->setHighlightSections(false);
    ui->TranslationView->horizontalHeader()->setBackgroundRole(QPalette::Background);


}

void MainWindow::Scene_Treeview_Init()
{
    QSqlQuery query(db);
    QList<QString> scene_mgr_id;
    QStandardItemModel* model1 = dynamic_cast<QStandardItemModel *>(ui->SceneTreeView->model());
    query.prepare("select * from scene_mgr where node_type = 1 order by pos asc");
    query.exec();
    while(query.next())
    {
        scene_mgr_id.append(query.value(0).toString());
        QString display = query.value(5).toString();
        model1->insertRows(model1->rowCount(), 1);
        QModelIndex index = model1->index(model1->rowCount()-1,0);
        model1->setData(index, display);
        ui->SceneTreeView->setModel(model1);
    }
    query.clear();
    for(int i = 0;i<scene_mgr_id.length();i++)
    {
        query.prepare("select * from scene_mgr where node_type = 0 and parent_id = :parent_id order by pos asc");
        query.bindValue(":parent_id",scene_mgr_id.at(i));
        query.exec();
        while(query.next())
        {
            QString display = query.value(5).toString();
            QStandardItem* itemChild = new QStandardItem(display);
            model1->itemFromIndex(model1->index(i,0))->appendRow(itemChild);
            ui->SceneTreeView->setModel(model1);
        }
    }
}

void MainWindow::on_pushButton_2_clicked()   //增加
{
   DBTableAddLine(ui->UserView);
}

void MainWindow::on_pushButton_3_clicked()  //删除
{
   DBTableDelLine(ui->UserView);
}

void MainWindow::on_pushButton_4_clicked()  //单行保存修改
{
    DBTableModifyLine(ui->UserView);
}

void MainWindow::Signal_Zone_Menu(const QPoint&)
{
    QMenu *cmenu = new QMenu(ui->SignalZoneView);
    if(ui->SignalZoneView->currentIndex().row() != -1)
    {
        QAction *Add_Signal_Zone = cmenu->addAction(tr("添加信号组"));
        QAction *Del_Signal_Zone = cmenu->addAction(tr("删除信号组"));
        connect(Add_Signal_Zone, SIGNAL(triggered(bool)), this, SLOT(Signal_Zone_Menu_Action1()));
        connect(Del_Signal_Zone, SIGNAL(triggered(bool)), this, SLOT(Signal_Zone_Menu_Action2()));
        cmenu->exec(QCursor::pos());//在当前鼠标位置显示
    }
    else
    {
        QAction *Add_Signal_Zone = cmenu->addAction(tr("添加信号组"));
        connect(Add_Signal_Zone, SIGNAL(triggered(bool)), this, SLOT(Signal_Zone_Menu_Action1()));
        cmenu->exec(QCursor::pos());//在当前鼠标位置显示
    }

}

void MainWindow::Signal_Zone_Menu_Action1()
{
    QString str = QInputDialog::getText(ui->SignalZoneView,tr("添加信号量分组"),tr("请输入名称："),QLineEdit::Normal,tr("newzone"));
    if(str != "")
    {
        QSqlQuery query(db);
        query.prepare("select * from signal_zone where sign_zone_name = :sign_zone_name");
        query.bindValue(":sign_zone_name",str);
        if(query.exec())
        {
            if(query.first())
            {
                QMessageBox::warning(this, tr("Warning"),tr("已存在相同信号分组！"));
                return;
            }
        }
        DBTableAddLine(ui->SignalZoneView);
        QAbstractItemModel *model = ui->SignalZoneView->model();
        model->setData(model->index(ui->SignalZoneView->model()->rowCount()-1,1),str);
        DBTableModifyLine(ui->SignalZoneView);

    }
}

void MainWindow::Signal_Zone_Menu_Action2()
{
    DBTableDelLine(ui->SignalZoneView);
}

void MainWindow::Signal_Menu(const QPoint&)
{
    QMenu *cmenu = new QMenu(ui->SignalView);
    if(ui->SignalZoneView->currentIndex().row() != -1)
    {
        if(ui->SignalView->currentIndex().row() != -1)
        {
            QAction *Add_Signal = cmenu->addAction(tr("添加信号"));
            QAction *Del_Signal = cmenu->addAction(tr("删除信号"));
            QAction *Mfy_Signal = cmenu->addAction(tr("修改信号"));
            connect(Add_Signal, SIGNAL(triggered(bool)), this, SLOT(Signal_Menu_Action1()));
            connect(Del_Signal, SIGNAL(triggered(bool)), this, SLOT(Signal_Menu_Action2()));
            connect(Mfy_Signal, SIGNAL(triggered(bool)), this, SLOT(Signal_Menu_Action3()));
            cmenu->exec(QCursor::pos());//在当前鼠标位置显示
        }
        else
        {
            QAction *Add_Signal = cmenu->addAction(tr("添加信号"));
            connect(Add_Signal, SIGNAL(triggered(bool)), this, SLOT(Signal_Menu_Action1()));
            cmenu->exec(QCursor::pos());//在当前鼠标位置显示
        }
    }
}

void MainWindow::Signal_Menu_Action1()
{
    ModifySignalFlag = false;
    AddSignalDialog * addsignaldialog = new AddSignalDialog(this);
    connect(addsignaldialog, &AddSignalDialog::AddInfoSignal, this, &MainWindow::Signal_Add_Info_Receive);
    addsignaldialog->exec();

}

void MainWindow::Signal_Menu_Action2()   //删除
{
    ModifySignalFlag = false;
    DBTableDelLine(ui->SignalView);
}

void MainWindow::Signal_Menu_Action3()   //修改
{
    ModifySignalFlag = true;
    AddInfo addinfo;
    QSqlTableModel *model = dynamic_cast<QSqlTableModel *>(ui->SignalView->model());
    addinfo.reg = model->data(model->index(ui->SignalView->currentIndex().row(),3)).toInt();
    addinfo.addr = model->data(model->index(ui->SignalView->currentIndex().row(),4)).toInt();
    addinfo.offset = model->data(model->index(ui->SignalView->currentIndex().row(),5)).toInt();
    addinfo.display = model->data(model->index(ui->SignalView->currentIndex().row(),6)).toString();
    addinfo.data_type = model->data(model->index(ui->SignalView->currentIndex().row(),7)).toInt();
    addinfo.uint = model->data(model->index(ui->SignalView->currentIndex().row(),8)).toString();
    addinfo.factor = model->data(model->index(ui->SignalView->currentIndex().row(),9)).toString();
    addinfo.range = model->data(model->index(ui->SignalView->currentIndex().row(),10)).toString();
    addinfo.property = model->data(model->index(ui->SignalView->currentIndex().row(),11)).toString();
    QVariant v;
    v.setValue(addinfo);
    AddSignalDialog w;
    connect(this,&MainWindow::Send_Modify_Signal_Info,&w,&AddSignalDialog::Modify_Signal_Info_Receive);
    connect(&w, &AddSignalDialog::AddInfoSignal, this, &MainWindow::Signal_Add_Info_Receive);
    emit Send_Modify_Signal_Info(v);
    w.exec();
}

void MainWindow::Scene_Zone_Menu(const QPoint&)
{
    QMenu *cmenu = new QMenu(ui->SceneZoneView);
    if(ui->SceneZoneView->currentIndex().row() != -1)
    {
        QAction *Add_Scene_Zone = cmenu->addAction(tr("添加"));
        QAction *Del_Scene_Zone = cmenu->addAction(tr("删除"));
        connect(Add_Scene_Zone, SIGNAL(triggered(bool)), this, SLOT(Scene_Zone_Menu_Action1()));
        connect(Del_Scene_Zone, SIGNAL(triggered(bool)), this, SLOT(Scene_Zone_Menu_Action2()));
        cmenu->exec(QCursor::pos());//在当前鼠标位置显示
    }
    else
    {
        QAction *Add_Scene_Zone = cmenu->addAction(tr("添加"));
        connect(Add_Scene_Zone, SIGNAL(triggered(bool)), this, SLOT(Scene_Zone_Menu_Action1()));
        cmenu->exec(QCursor::pos());//在当前鼠标位置显示
    }

}

void MainWindow::Scene_Zone_Menu_Action1()
{
    AddSceneZoneDialog *addscenezonedialog = new AddSceneZoneDialog(this);
    connect(addscenezonedialog, &AddSceneZoneDialog::Send_AddSceneZoneInfo, this, &MainWindow::Scene_Zone_Add_Info_Receive);
    addscenezonedialog->exec();
}

void MainWindow::Scene_Zone_Menu_Action2()
{
    DBTableDelLine(ui->SceneZoneView);
}

void MainWindow::Scene_Tree_Menu(const QPoint& )
{
    QMenu *cmenu = new QMenu(ui->SceneTreeView);
    if(row_mouse_catch_index != -1)        //判断是否在树上右击
    {
        QAction *Add_Scene_Tree = cmenu->addAction(tr("添加画面"));
        QAction *Del_Scene_Tree = cmenu->addAction(tr("删除"));
        //QAction *Add_Node = cmenu->addAction("添加节点");
        connect(Add_Scene_Tree, SIGNAL(triggered(bool)), this, SLOT(Scene_Tree_Menu_Action1()));
        connect(Del_Scene_Tree, SIGNAL(triggered(bool)), this, SLOT(Scene_Tree_Menu_Action2()));
        //connect(Add_Node, SIGNAL(triggered(bool)), this, SLOT(Scene_Tree_Menu_Action3()));
        cmenu->exec(QCursor::pos());//在当前鼠标位置显示
    }
    else
    {
        QAction *Add_Node = cmenu->addAction(tr("添加节点"));
        connect(Add_Node, SIGNAL(triggered(bool)), this, SLOT(Scene_Tree_Menu_Action3()));
        cmenu->exec(QCursor::pos());//在当前鼠标位置显示
    }
    row_mouse_catch_index = -1;
}

void MainWindow::Scene_Tree_Menu_Action1()  //添加画面
{  
    SceneMgrDialog *scenemgrdialog = new SceneMgrDialog(this);
    connect(scenemgrdialog, &SceneMgrDialog::Send_Scene_Choose_Info, this, &MainWindow::Send_Scene_Choose_Info_Receive);
    scenemgrdialog->exec();
}

void MainWindow::Scene_Tree_Menu_Action2()  //删除
{
    QSqlQuery query(db);
    QVariant scene_mgr_id_value;
    query.prepare("select * from scene_mgr where display = :display and node_type = :node_type");
    query.bindValue(":display",getTopParent(ui->SceneTreeView->currentIndex()).data().toString());
    query.bindValue(":node_type",1);
    if(query.exec())
    {
        if(query.first())
        {
            QSqlRecord rec = query.record();
            scene_mgr_id_value = query.value(rec.indexOf("scene_mgr_id"));
            qDebug()<<scene_mgr_id_value.toInt();
        }
    }

    query.clear();
    if(!ui->SceneTreeView->currentIndex().parent().isValid())  //根节点删除
    {
        query.prepare("delete from scene_mgr where scene_mgr_id = :scene_mgr_id or parent_id = :parent_id");
        query.bindValue(":scene_mgr_id",scene_mgr_id_value);
        query.bindValue(":parent_id",scene_mgr_id_value);
        query.exec();
    }
    else   //子节点画面删除
    {
        query.prepare("delete from scene_mgr where display = :display and parent_id = :parent_id");
        query.bindValue(":display",ui->SceneTreeView->currentIndex().data().toString());
        query.bindValue(":parent_id",scene_mgr_id_value);
        query.exec();
    }
    QStandardItemModel* model = dynamic_cast<QStandardItemModel *>(ui->SceneTreeView->model());
    model->removeRow(ui->SceneTreeView->currentIndex().row(),ui->SceneTreeView->currentIndex().parent());

}

void MainWindow::Scene_Tree_Menu_Action3()  //添加节点
{
    QString str = QInputDialog::getText(this,tr("添加节点"),tr("请输入名称："),QLineEdit::Normal);
    QStandardItemModel* model = dynamic_cast<QStandardItemModel *>(ui->SceneTreeView->model());

        if(str != "")
        {
            QSqlQuery query(db);
            query.prepare("select * from scene_mgr where node_type = 1 and display = :display");
            query.bindValue(":display",str);
            if(query.exec())
            {
                if(query.first())
                {
                    QMessageBox::warning(this, tr("Warning"),tr("已存在相同节点！"));
                    return;
                }
            }
            query.prepare("insert into scene_mgr (node_type,pos,display) values(:node_type,:pos,:display)");
            query.bindValue(":node_type", 1);
            query.bindValue(":pos", model->rowCount());
            query.bindValue(":display", str);
            query.exec();

            model->insertRows(model->rowCount(), 1);
            QModelIndex index = model->index(model->rowCount()-1,0);
            model->setData(index, str);
            ui->SceneTreeView->setModel(model);

        }
    row_mouse_catch_index1 = -1;
    row_mouse_catch_index = -1;
}

void MainWindow::Signal_Add_Info_Receive(QVariant& v)       //添加信号信息接收
{
    AddInfo addinfo;
    addinfo = v.value<AddInfo>();
    QSqlQuery query(db);
    query.prepare("select * from signal where display = :display");
    query.bindValue(":display",addinfo.display);
    if(query.exec())
    {
        if(query.first())
        {
            QMessageBox::warning(this, tr("Warning"),tr("已存在相同信号名称！"));
            return;
        }
    }
    if(ModifySignalFlag == true)     //修改
    {
        ModifySignalFlag = false;
        QAbstractItemModel *model = ui->SignalView->model();
        model->setData(model->index(ui->SignalView->currentIndex().row(),3),addinfo.reg);
        model->setData(model->index(ui->SignalView->currentIndex().row(),4),addinfo.addr);
        model->setData(model->index(ui->SignalView->currentIndex().row(),5),addinfo.offset);
        model->setData(model->index(ui->SignalView->currentIndex().row(),6),addinfo.display);
        model->setData(model->index(ui->SignalView->currentIndex().row(),7),addinfo.data_type);
        model->setData(model->index(ui->SignalView->currentIndex().row(),8),addinfo.uint);
        model->setData(model->index(ui->SignalView->currentIndex().row(),9),addinfo.factor);
        model->setData(model->index(ui->SignalView->currentIndex().row(),10),addinfo.range);
        model->setData(model->index(ui->SignalView->currentIndex().row(),11),addinfo.property);
        DBTableModifyLine(ui->SignalView);
    }
    else                            //添加
    {
        QSqlTableModel *Mode = dynamic_cast<QSqlTableModel *>(ui->SignalView->model());
        QString Mode_Filter_pre = Mode->filter();
        DBTableAddLine(ui->SignalView);
        QAbstractItemModel *model = ui->SignalView->model();
        model->setData(model->index(ui->SignalView->model()->rowCount()-1,1),ui->SignalZoneView->currentIndex().row());
        model->setData(model->index(ui->SignalView->model()->rowCount()-1,2),ui->SignalZoneView->currentIndex().data());
        model->setData(model->index(ui->SignalView->model()->rowCount()-1,3),addinfo.reg);
        model->setData(model->index(ui->SignalView->model()->rowCount()-1,4),addinfo.addr);
        model->setData(model->index(ui->SignalView->model()->rowCount()-1,5),addinfo.offset);
        model->setData(model->index(ui->SignalView->model()->rowCount()-1,6),addinfo.display);
        model->setData(model->index(ui->SignalView->model()->rowCount()-1,7),addinfo.data_type);
        model->setData(model->index(ui->SignalView->model()->rowCount()-1,8),addinfo.uint);
        model->setData(model->index(ui->SignalView->model()->rowCount()-1,9),addinfo.factor);
        model->setData(model->index(ui->SignalView->model()->rowCount()-1,10),addinfo.range);
        model->setData(model->index(ui->SignalView->model()->rowCount()-1,11),addinfo.property);
        DBTableModifyLine(ui->SignalView);
        Mode->setFilter(Mode_Filter_pre);
    }
}

void MainWindow::Scene_Zone_Add_Info_Receive(QVariant& v)      //添加画面分组接收
{
    AddSceneZoneInfo addscenezoneinfo;
    addscenezoneinfo = v.value<AddSceneZoneInfo>();
    QSqlQuery query(db);
    query.prepare("select * from scene_zone where display = :display");
    query.bindValue(":display",addscenezoneinfo.display);
    if(query.exec())
    {
        if(query.first())
        {
            QMessageBox::warning(this, tr("Warning"),tr("已存在相同画面分组！"));
            return;
        }
    }
    DBTableAddLine(ui->SceneZoneView);
    QAbstractItemModel *model = ui->SceneZoneView->model();
    model->setData(model->index(ui->SceneZoneView->model()->rowCount()-1,1),addscenezoneinfo.type);
    model->setData(model->index(ui->SceneZoneView->model()->rowCount()-1,2),addscenezoneinfo.label);
    model->setData(model->index(ui->SceneZoneView->model()->rowCount()-1,3),addscenezoneinfo.display);
    DBTableModifyLine(ui->SceneZoneView);
}

void MainWindow::DBTableAddLine(QTableView* view)
{
    QSqlTableModel *pMode = dynamic_cast<QSqlTableModel *>(view->model());
    pMode->setFilter("");
    pMode->setSort(0,Qt::SortOrder::AscendingOrder);
    QSqlRecord record = pMode->record();
    pMode->insertRecord(pMode->rowCount(),record);
    // 每次手动提交，都会重新刷新tableView，保持mode和tableView一致
    pMode->submitAll();
}

void MainWindow::DBTableDelLine(QTableView* view)
{
    QSqlTableModel *pMode = dynamic_cast<QSqlTableModel *>(view->model());
    // 当QSqlTableModel::EditStrategy 选择 非QSqlTableModel::OnManualSubmit 时,
    // 每次删除都可删除掉model，但是tableview那一列为空，一直在，只好设置为 QSqlTableModel::OnManualSubmit
    pMode->removeRow(view->currentIndex().row());
    // 每次手动提交，都会重新刷新tableView，保持mode和tableView一致
    pMode->submitAll();
}

void MainWindow::DBTableModifyLine(QTableView* view)
{
    QSqlTableModel *pMode = dynamic_cast<QSqlTableModel *>(view->model());
    pMode->database().transaction(); //开始事务操作
    if (pMode->submitAll()) // 提交所有被修改的数据到数据库中
    {
        pMode->database().commit(); //提交成功，事务将真正修改数据库数据
    }
    else
    {
        pMode->database().rollback(); //提交失败，事务回滚
        QMessageBox::warning(this, tr("tableModel"),tr("数据库错误"));
    }
    pMode->revertAll(); //撤销修改s
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousepos = event->pos();
    if(ui->stackedWidget->currentIndex() == 1)
    {
        if((mousepos.rx() > (ui->SignalZoneView->width()+ui->stackedWidget->x()+1))&&(mousepos.rx() < (ui->SignalZoneView->width()+ui->stackedWidget->x()+4)))
        {
            this->setCursor(Qt::SplitHCursor);
            ui->SignalZoneView->setAttribute(Qt::WA_TransparentForMouseEvents,true);
            ui->SignalView->setAttribute(Qt::WA_TransparentForMouseEvents,true);
        }
        else
        {
            this->setCursor(Qt::ArrowCursor);
            ui->SignalZoneView->setAttribute(Qt::WA_TransparentForMouseEvents,false);
            ui->SignalView->setAttribute(Qt::WA_TransparentForMouseEvents,false);
        }
    }

    if(ui->stackedWidget->currentIndex() == 2)
    {
        if((mousepos.rx() > (ui->SceneZoneView->width()+ui->stackedWidget->x()+1))&&(mousepos.rx() < (ui->SceneZoneView->width()+ui->stackedWidget->x()+4)))
        {
            this->setCursor(Qt::SplitHCursor);
            ui->SceneZoneView->setAttribute(Qt::WA_TransparentForMouseEvents,true);
            ui->groupBox_1->setAttribute(Qt::WA_TransparentForMouseEvents,true);
            ui->groupBox_2->setAttribute(Qt::WA_TransparentForMouseEvents,true);
        }
        else if((mousepos.ry() > (ui->groupBox_1->height()+ui->stackedWidget->y()+1))&&(mousepos.ry() < (ui->groupBox_1->height()+ui->stackedWidget->y()+4)))
        {
            this->setCursor(Qt::SplitVCursor);
            ui->SceneZoneView->setAttribute(Qt::WA_TransparentForMouseEvents,true);
            ui->groupBox_1->setAttribute(Qt::WA_TransparentForMouseEvents,true);
            ui->groupBox_2->setAttribute(Qt::WA_TransparentForMouseEvents,true);
        }
        else
        {
            this->setCursor(Qt::ArrowCursor);
            ui->SceneZoneView->setAttribute(Qt::WA_TransparentForMouseEvents,false);
            ui->groupBox_1->setAttribute(Qt::WA_TransparentForMouseEvents,false);
            ui->groupBox_2->setAttribute(Qt::WA_TransparentForMouseEvents,false);
        }
    }

    if(mouseFlagX){
        int xChanged = event->x()-mousePosX;
        if(ui->stackedWidget->currentIndex() == 1)
        {
            ui->SignalZoneView->resize(ui->SignalZoneView->geometry().width()+xChanged,ui->SignalZoneView->geometry().height());
            ui->SignalView->setGeometry(ui->SignalZoneView->width()+5,0,ui->stackedWidget->width()-ui->SignalZoneView->width(),ui->stackedWidget->height());
        }
        if(ui->stackedWidget->currentIndex() == 2)
        {
            ui->SceneZoneView->resize(ui->SceneZoneView->geometry().width()+xChanged,ui->SceneZoneView->geometry().height());
            ui->groupBox_1->setGeometry(ui->SceneZoneView->width()+5,0,ui->stackedWidget->width()-ui->SceneZoneView->width(),ui->groupBox_1->height());
            ui->groupBox_2->setGeometry(ui->SceneZoneView->width()+5,ui->groupBox_1->height()+5,ui->stackedWidget->width()-ui->SceneZoneView->width(),ui->groupBox_2->height());
        }
        mousePosX = event->x();
    }
    if(mouseFlagY){
        int yChanged = event->y()-mousePosY;
        if(ui->stackedWidget->currentIndex() == 2)
        {
            ui->groupBox_1->resize(ui->groupBox_1->width(),ui->groupBox_1->height()+yChanged);
            ui->groupBox_2->setGeometry(ui->groupBox_2->x(),ui->groupBox_2->y()+yChanged,ui->groupBox_2->width(),ui->groupBox_2->height()-yChanged);
        }
        mousePosY = event->y();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(ui->stackedWidget->currentIndex() == 1)
    {
        if((event->x() > (ui->SignalZoneView->width()+ui->stackedWidget->x()+1))&&(event->x() < (ui->SignalZoneView->width()+ui->stackedWidget->x()+4)))
        {
            mousePosX = event->x();
            mouseFlagX = true;
        }
    }
    if(ui->stackedWidget->currentIndex() == 2)
    {
        if((event->x() > (ui->SceneZoneView->width()+ui->stackedWidget->x()+1))&&(event->x() < (ui->SceneZoneView->width()+ui->stackedWidget->x()+4)))
        {
            mousePosX = event->x();
            mouseFlagX = true;
        }
        if((event->y() > (ui->groupBox_1->height()+ui->stackedWidget->y()+1))&&(event->y() < (ui->groupBox_1->height()+ui->stackedWidget->y()+4)))
        {
            mousePosY = event->y();
            mouseFlagY = true;
        }
    }

}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    mouseFlagX = false;
    mouseFlagY = false;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->SignalZoneView->setGeometry(0,0,ui->stackedWidget->width()/6,ui->stackedWidget->height());
    ui->SignalView->setGeometry(ui->SignalZoneView->width()+5,0,ui->stackedWidget->width()-ui->SignalZoneView->width(),ui->stackedWidget->height());
    ui->SceneZoneView->setGeometry(0,0,ui->stackedWidget->width()/6,ui->stackedWidget->height());
    ui->groupBox_1->setGeometry(ui->SceneZoneView->width()+5,0,ui->stackedWidget->width()-ui->SceneZoneView->width(),ui->stackedWidget->height()/4);
    ui->groupBox_2->setGeometry(ui->SceneZoneView->width()+5,ui->groupBox_1->height()+5,ui->stackedWidget->width()-ui->SceneZoneView->width(),ui->stackedWidget->height()-ui->groupBox_1->height());

}


void MainWindow::on_SignalZoneView_clicked(const QModelIndex &index)
{
    QSqlTableModel *Mode = dynamic_cast<QSqlTableModel *>(ui->SignalView->model());
    Mode->setFilter("sign_zone_id = "+QString::number(index.row()));
}

void MainWindow::on_AddButton_clicked()  //Add Scene Items
{
    if(ui->SceneZoneView->currentIndex().row() != -1)
    {
        AddSceneItemDialog *addsceneitemdialog = new AddSceneItemDialog(this);
        connect(addsceneitemdialog, &AddSceneItemDialog::Send_AddSceneItemsInfo, this, &MainWindow::Scene_Item_Add_Info_Receive);
        addsceneitemdialog->exec();
    }
    else
    {
        QMessageBox::warning(this, tr("Warning"),tr("请先选择画面！"));
        return;
    }

}

void MainWindow::on_DelButton_clicked()  //Del Scene Items
{
    if(ui->SceneZoneView->currentIndex().row() != -1)
    {
        if(ui->SceneView->currentIndex().row() != -1)
        {
            QAbstractItemModel *model = ui->SceneView->model();
            int row_curr = ui->SceneView->currentIndex().row();  //删除行号
            DBTableDelLine(ui->SceneView);
            for(int i = row_curr;i <= model->rowCount(); i++)
            {
                model->setData(model->index(i,5),model->index(i,5).data().toInt()-1);
            }
            DBTableModifyLine(ui->SceneView);
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Warning"),tr("请先选择画面！"));
        return;
    }

}

void MainWindow::on_UpButton_clicked()  //up
{
    if(ui->SceneZoneView->currentIndex().row() != -1)
    {
        if(ui->SceneView->currentIndex().row() != -1)
        {
            QAbstractItemModel *model = ui->SceneView->model();
            if((model->index(ui->SceneView->currentIndex().row(),5).data().toInt()-1) >= 0)
            {
                model->setData(model->index(ui->SceneView->currentIndex().row(),5),model->index(ui->SceneView->currentIndex().row(),5).data().toInt()-1);
                model->setData(model->index(ui->SceneView->currentIndex().row()-1,5),model->index(ui->SceneView->currentIndex().row(),5).data().toInt()+1);
                DBTableModifyLine(ui->SceneView);
            }
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Warning"),tr("请先选择画面！"));
        return;
    }
}

void MainWindow::on_DownButton_clicked()  //down
{
    if(ui->SceneZoneView->currentIndex().row() != -1)
    {
        if(ui->SceneView->currentIndex().row() != -1)
        {
            QAbstractItemModel *model = ui->SceneView->model();

            model->setData(model->index(ui->SceneView->currentIndex().row(),5),model->index(ui->SceneView->currentIndex().row(),5).data().toInt()+1);
            model->setData(model->index(ui->SceneView->currentIndex().row()+1,5),model->index(ui->SceneView->currentIndex().row(),5).data().toInt()-1);
            DBTableModifyLine(ui->SceneView);
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Warning"),tr("请先选择画面！"));
        return;
    }
}

void MainWindow::on_SaveButton_clicked() //Save
{
    DBTableModifyLine(ui->SceneView);
}

void MainWindow::Scene_Item_Add_Info_Receive(QString& str)
{
    QString sign_id,display;
    QAbstractItemModel *model = ui->SceneView->model();
    QSqlTableModel *Mode = dynamic_cast<QSqlTableModel *>(ui->SceneView->model());
    int Scene_row_cnt = ui->SceneView->model()->rowCount();  //当前分组内容条数
    QString Mode_Filter_pre = Mode->filter();
    while(str.indexOf("(")!=-1)
    {
        int start = str.indexOf("(");
        int comma = str.indexOf(",");
        int end = str.indexOf(")");
        sign_id = str.mid(start+1,comma-start-1);
        display = str.mid(comma+1,end-comma-1);
        str = str.remove(0,end+1);
        DBTableAddLine(ui->SceneView);
        QVariant scene_zone_id_value;
        QSqlQuery query(db);
        query.prepare("select * from scene_zone where display = :display");
        query.bindValue(":display",ui->SceneZoneView->currentIndex().data().toString());
        if(query.exec())
        {
            if(query.first())
            {
                QSqlRecord rec = query.record();
                scene_zone_id_value = query.value(rec.indexOf("scene_zone_id"));
            }
        }
        query.clear();
        model->setData(model->index(ui->SceneView->model()->rowCount()-1,1),scene_zone_id_value);
        model->setData(model->index(ui->SceneView->model()->rowCount()-1,3),display);
        model->setData(model->index(ui->SceneView->model()->rowCount()-1,4),sign_id);
        model->setData(model->index(ui->SceneView->model()->rowCount()-1,5),Scene_row_cnt++);

        query.prepare("select * from signal where sign_id = :sign_id");
        query.bindValue(":sign_id",sign_id);
        if(query.exec())
        {
            if(query.first())
            {
                QSqlRecord rec = query.record();
                if(query.value(rec.indexOf("property")).toString() != "")
                {
                    model->setData(model->index(ui->SceneView->model()->rowCount()-1,6),1);
                }
            }
        }
        DBTableModifyLine(ui->SceneView);
    }
    Mode->setFilter(Mode_Filter_pre);
}

void MainWindow::Send_Scene_Choose_Info_Receive(QString& str)        //节点添加画面内容接收
{
    QStandardItemModel* model = dynamic_cast<QStandardItemModel *>(ui->SceneTreeView->model());
    QSqlQuery query(db);
    int scene_mgr_id_col,scene_zone_id_col;
    QVariant scene_mgr_id_value,scene_zone_id_value,pos_value;
    query.prepare("select * from scene_mgr where node_type = 0 and display = :display");
    query.bindValue(":display",str);
    if(query.exec())
    {
        if(query.first())
        {
            QMessageBox::warning(this, tr("Warning"),tr("已存在相同画面！"));
            return;
        }
    }
    query.prepare("select * from scene_zone where display = :display");
    query.bindValue(":display",str);
    if(query.exec())
    {
        if(query.first())
        {
            QSqlRecord rec = query.record();
            scene_zone_id_col = rec.indexOf("scene_zone_id");
            scene_zone_id_value = query.value(scene_zone_id_col);
        }
    }
    query.clear();
    query.prepare("select * from scene_mgr where display = :display and pos = :pos");
    query.bindValue(":display",getTopParent(ui->SceneTreeView->currentIndex()).data().toString());   // 绑定数据到指定的位置
    query.bindValue(":pos",QString::number(ui->SceneTreeView->currentIndex().row()));
    if(query.exec())
    {
        if(query.first())
        {
            QSqlRecord rec = query.record();
            scene_mgr_id_col = rec.indexOf("scene_mgr_id");
            scene_mgr_id_value = query.value(scene_mgr_id_col);
        }
    }
    query.clear();
    query.prepare("select * from scene_mgr where parent_id = :parent_id order by pos desc");
    query.bindValue(":parent_id",scene_mgr_id_value);
    if(query.exec())
    {
        if(query.first())
        {
            QSqlRecord rec = query.record();
            pos_value = query.value(rec.indexOf("pos"));
        }
        else
        {
            pos_value = -1;
        }
    }
    query.clear();
    query.prepare("insert into scene_mgr (node_type, parent_id,scene_id,pos,display) values(:node_type, :parent_id,:scene_id,:pos,:display)");
    query.bindValue(":node_type", 0);
    query.bindValue(":parent_id", scene_mgr_id_value);
    query.bindValue(":scene_id", scene_zone_id_value);
    query.bindValue(":pos", pos_value.toInt()+1);
    query.bindValue(":display", str);
    query.exec();
    QStandardItem* itemChild = new QStandardItem(str);
    getTopParent(model->itemFromIndex(ui->SceneTreeView->currentIndex()))->appendRow(itemChild);
    ui->SceneTreeView->setModel(model);
}

void MainWindow::row_mouse_catch(const QModelIndex & index)
{
    row_mouse_catch_index = index.row();
    row_mouse_catch_index1 = index.row();
}


void MainWindow::on_SceneZoneView_clicked(const QModelIndex &index)
{
    QVariant scene_zone_id_value;
    QSqlTableModel *Mode = dynamic_cast<QSqlTableModel *>(ui->SceneView->model());
    Mode->setSort(5,Qt::SortOrder::AscendingOrder);
    QSqlQuery query(db);
    query.prepare("select * from scene_zone where display = :display");
    query.bindValue(":display",index.data().toString());
    if(query.exec())
    {
        if(query.first())
        {
            QSqlRecord rec = query.record();
            scene_zone_id_value = query.value(rec.indexOf("scene_zone_id"));
        }
    }
    Mode->setFilter("scene_zone_id = "+scene_zone_id_value.toString());

}



void MainWindow::on_SceneView_clicked(const QModelIndex &index)
{
    QSqlTableModel *Mode = dynamic_cast<QSqlTableModel *>(ui->Scene_Signal_Info_View->model());
    Mode->setFilter("sign_id = "+ui->SceneView->model()->data(ui->SceneView->model()->index(index.row(),4)).toString());
}

QStandardItem* MainWindow::getTopParent(QStandardItem* item)
{
    QStandardItem* secondItem = item;
    while(item->parent()!= 0)
    {
        secondItem = item->parent();
        item = secondItem;
    }
    if(secondItem->index().column() != 0)
    {
         QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->SceneTreeView->model());
         secondItem = model->itemFromIndex(secondItem->index().sibling(secondItem->index().row(),0));
    }
    return secondItem;
}

QModelIndex MainWindow::getTopParent(QModelIndex itemIndex)
{
    QModelIndex secondItem = itemIndex;
    while(itemIndex.parent().isValid())
    {
        secondItem = itemIndex.parent();
        itemIndex = secondItem;
    }
    if(secondItem.column() != 0)
    {
         secondItem = secondItem.sibling(secondItem.row(),0);
    }
    return secondItem;
}



void MainWindow::on_SyButton_clicked()    //上移(采用pos交换方式，避免排序存在空缺的影响)
{
    if(ui->SceneTreeView->currentIndex().isValid())
    {
        QModelIndex index_pre = getTopParent(ui->SceneTreeView->currentIndex());
        QSqlQuery query(db);
        QList<uint> pos_list;
        QList<QString> display_list;
        QVariant node_type;
        QVariant parent_id;
        query.prepare("select * from scene_mgr where display = :display");
        query.bindValue(":display",ui->SceneTreeView->currentIndex().data());
        if(query.exec())
        {
            if(query.first())
            {
                node_type = query.value(1);
                if(node_type.toUInt() == 0)
                    parent_id = query.value(2);
            }
        }
        if(node_type.toUInt() == 0)
        {
            query.prepare("select * from scene_mgr where node_type = :node_type and parent_id = :parent_id order by pos asc");
            query.bindValue(":node_type",node_type);
            query.bindValue(":parent_id",parent_id);
        }
        else
        {
            query.prepare("select * from scene_mgr where node_type = :node_type order by pos asc");
            query.bindValue(":node_type",node_type);
        }
        if(query.exec())
        {
            while (query.next())
            {
                display_list.append(query.value(5).toString());
                pos_list.append(query.value(4).toUInt());
            }
        }
      int i = display_list.indexOf(ui->SceneTreeView->currentIndex().data().toString());
      if(i == 0)
          return;
      query.prepare("update scene_mgr set pos = :pos where display = :display and node_type = :node_type");
      query.bindValue(":pos",pos_list.at(i-1));
      query.bindValue(":display",display_list.at(i));
      query.bindValue(":node_type",node_type);
      query.exec();
      query.clear();
      query.prepare("update scene_mgr set pos = :pos where display = :display and node_type = :node_type");
      query.bindValue(":pos",pos_list.at(i));
      query.bindValue(":display",display_list.at(i-1));
      query.bindValue(":node_type",node_type);
      query.exec();
      int count = ui->SceneTreeView->model()->rowCount();
      ui->SceneTreeView->model()->removeRows(0,count);
      Scene_Treeview_Init();
      if(node_type == 0)
        ui->SceneTreeView->expand(index_pre);
    }
}

void MainWindow::on_XyButton_clicked()    //下移
{
    if(ui->SceneTreeView->currentIndex().isValid())
    {
        QModelIndex index_pre = getTopParent(ui->SceneTreeView->currentIndex());
        QSqlQuery query(db);
        QList<uint> pos_list;
        QList<QString> display_list;
        QVariant node_type;
        QVariant parent_id;
        query.prepare("select * from scene_mgr where display = :display");
        query.bindValue(":display",ui->SceneTreeView->currentIndex().data());
        if(query.exec())
        {
            if(query.first())
            {
                node_type = query.value(1);
                if(node_type.toUInt() == 0)
                    parent_id = query.value(2);
            }
        }
        if(node_type.toUInt() == 0)
        {
            query.prepare("select * from scene_mgr where node_type = :node_type and parent_id = :parent_id order by pos asc");
            query.bindValue(":node_type",node_type);
            query.bindValue(":parent_id",parent_id);
        }
        else
        {
            query.prepare("select * from scene_mgr where node_type = :node_type order by pos asc");
            query.bindValue(":node_type",node_type);
        }
        if(query.exec())
        {
            while (query.next())
            {
                display_list.append(query.value(5).toString());
                pos_list.append(query.value(4).toUInt());
            }
        }
      int i = display_list.indexOf(ui->SceneTreeView->currentIndex().data().toString());
      if(i == display_list.length()-1)
          return;
      query.prepare("update scene_mgr set pos = :pos where display = :display and node_type = :node_type");
      query.bindValue(":pos",pos_list.at(i+1));
      query.bindValue(":display",display_list.at(i));
      query.bindValue(":node_type",node_type);
      query.exec();
      query.clear();
      query.prepare("update scene_mgr set pos = :pos where display = :display and node_type = :node_type");
      query.bindValue(":pos",pos_list.at(i));
      query.bindValue(":display",display_list.at(i+1));
      query.bindValue(":node_type",node_type);
      query.exec();
      int count = ui->SceneTreeView->model()->rowCount();
      ui->SceneTreeView->model()->removeRows(0,count);
      Scene_Treeview_Init();
      if(node_type == 0)
        ui->SceneTreeView->expand(index_pre);
    }
}

void MainWindow::on_UpdateButton_clicked()  //更新翻译
{
    QSqlTableModel *Model = dynamic_cast<QSqlTableModel *>(ui->TranslationView->model());
    int i= ui->TranslationView->model()->rowCount();
    QSqlQuery query(db);
    QAbstractItemModel *model = ui->TranslationView->model();
    query.prepare("select * from scene_mgr");
    if(query.exec())
    {
        while(query.next())
        {
            QSqlQuery squery(db);
            squery.prepare("select * from translation where tablename like 'scene_mgr' and table_item_id = :table_item_id");   //翻译
            squery.bindValue(":table_item_id",query.value(0));
            if(squery.exec())
            {
                if(squery.first())
                {
                    //
                }
                else
                {
                    model->insertRow(i++);
                    model->setData(model->index(Model->rowCount()-1,0),"scene_mgr");
                    model->setData(model->index(Model->rowCount()-1,1),query.value(0));
                    model->setData(model->index(Model->rowCount()-1,2),"display");
                    model->setData(model->index(Model->rowCount()-1,3),query.value(5));
                }
            }
        }
    }
    query.clear();
    query.prepare("select * from scene_zone");
    if(query.exec())
    {
        while(query.next())
        {
            QSqlQuery squery(db);
            squery.prepare("select * from translation where tablename like 'scene_zone' and table_item_id = :table_item_id");   //翻译
            squery.bindValue(":table_item_id",query.value(0));
            if(squery.exec())
            {
                if(squery.first())
                {
                    //
                }
                else
                {
                    model->insertRow(i++);
                    model->setData(model->index(Model->rowCount()-1,0),"scene_zone");
                    model->setData(model->index(Model->rowCount()-1,1),query.value(0));
                    model->setData(model->index(Model->rowCount()-1,2),"display");
                    model->setData(model->index(Model->rowCount()-1,3),query.value(3));
                }
            }

        }
    }
    query.clear();
    query.prepare("select * from signal");
    if(query.exec())
    {
        while(query.next())
        {
            QSqlQuery squery(db);
            squery.prepare("select * from translation where tablename like 'signal' and table_field like 'display' and table_item_id = :table_item_id");   //翻译
            squery.bindValue(":table_item_id",query.value(0));
            if(squery.exec())
            {
                if(squery.first())
                {
                    //
                }
                else
                {
                    model->insertRow(i++);
                    model->setData(model->index(Model->rowCount()-1,0),"signal");
                    model->setData(model->index(Model->rowCount()-1,1),query.value(0));
                    model->setData(model->index(Model->rowCount()-1,2),"display");
                    model->setData(model->index(Model->rowCount()-1,3),query.value(6));
                }
            }
        }
    }
    query.clear();
    query.prepare("select * from signal where property IS NOT NULL");
    if(query.exec())
    {
        while(query.next())
        {
            QSqlQuery squery(db);
            squery.prepare("select * from translation where tablename like 'signal' and table_field like 'property' and table_item_id = :table_item_id");   //翻译
            squery.bindValue(":table_item_id",query.value(0));
            if(squery.exec())
            {
                if(squery.first())
                {
                    //
                }
                else
                {
                    model->insertRow(i++);
                    model->setData(model->index(Model->rowCount()-1,0),"signal");
                    model->setData(model->index(Model->rowCount()-1,1),query.value(0));
                    model->setData(model->index(Model->rowCount()-1,2),"property");
                    model->setData(model->index(Model->rowCount()-1,3),query.value(11));
                }
            }

        }
    }
    query.clear();
}

void MainWindow::on_ReleaseButton_clicked()  //发布翻译
{
    DBTableModifyLine(ui->TranslationView);
}
