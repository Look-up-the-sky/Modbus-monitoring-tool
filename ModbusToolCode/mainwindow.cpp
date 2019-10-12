#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QMessageBox>
#include <QInputDialog>
#include "logindialog.h"

uint LoginDialog::Language;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    scene_items_num_list.clear();
    Signal_info_list.clear();
    ui->setupUi(this);
    if(LoginDialog::Language == English)
    {
        translator.load(":/language/T_I18N_en_UK.qm");
        QApplication::instance()->installTranslator(&translator);
        ui->retranslateUi(this);
    }
    serial = new Serial();
    thread = new QThread;
    serial->moveToThread(thread);
    progressbar = new QProgressBar(this);
    QSize *size = new QSize(250,30);
    progressbar->setMaximumSize(*size);
    progressbar->setMinimum(0);
    progressbar->setMaximum(100);
    curr_progressbar_maxinum = 100;
    progressbar->setValue(0);
    progressbar->setFormat(tr("未连接"));
    progressbar->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);  // 对齐方式
    ui->statusBar->addPermanentWidget(progressbar);
    //this->setGeometry(QApplication::desktop()->availableGeometry());

    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("Service"));
    ui->treeView->setModel(model);
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStandardItemModel *itemmodel = new QStandardItemModel(ui->tableView);
    itemmodel->setColumnCount(4);//设置列表列数

    /*---设置列表每一列表头的显示内容----*/
    itemmodel->setHeaderData(0, Qt::Horizontal, tr("名称"));
    itemmodel->setHeaderData(1, Qt::Horizontal, tr("显示"));
    itemmodel->setHeaderData(2, Qt::Horizontal, tr("单位"));
    itemmodel->setHeaderData(3, Qt::Horizontal, tr("范围"));

    ui->tableView->setModel(itemmodel);

    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    //ui->tableView->setSortingEnabled(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    //ui->tableView->horizontalHeader()->sortIndicatorOrder();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setSectionsClickable(true);
    ui->tableView->horizontalHeader()->setHighlightSections(false);
    ui->tableView->horizontalHeader()->setBackgroundRole(QPalette::Background);

    connect(this,&MainWindow::Doworks_Signal,serial,&Serial::doworks);
    connect(this,&MainWindow::Stop_Thread_Signal,serial,&Serial::Stop_Thread);
    connect(this,&MainWindow::Open_Com_Signal,serial,&Serial::open_com);
    connect(serial,&Serial::Data_Updata_Signal,this,&MainWindow::Data_Updata_Slot);
    thread->start();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_CommAction_triggered()
{
    CommSetDialog * commsetdialog = new CommSetDialog(this);
    connect(commsetdialog,&CommSetDialog::Send_Comm_Set_Info,this,&MainWindow::Comm_Set_Info_Receive);
    commsetdialog->exec();
}

void MainWindow::Comm_Set_Info_Receive(QVariant& v)
{
    commsetinfo = v.value<CommSetInfo>();
    addr = commsetinfo.addr;
    outtime = commsetinfo.outtime;
    com = commsetinfo.com;
    Baud = commsetinfo.Baud;
    emit Open_Com_Signal();
}

void MainWindow::closeEvent(QCloseEvent *)
{

    emit CloseSignal();
    this->deleteLater();
}

void MainWindow::DbNameSlot(QString arg)
{
    DbName = arg;
    qDebug()<<DbName;
    init();
}

void MainWindow::init()
{
    QList<QString> scene_mgr_id;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./Config/"+DbName+".db");
    ui->treeView->model()->removeRows(0,ui->treeView->model()->rowCount());
    if(db.open())
    {

        QSqlQuery query(db);
        QStandardItemModel* model = dynamic_cast<QStandardItemModel *>(ui->treeView->model());
        query.prepare("select * from scene_mgr where node_type = 1 order by pos asc");       //节点
        query.exec();
        while(query.next())
        {
            scene_mgr_id.append(query.value(0).toString());
            if(LoginDialog::Language == Chinese)
            {
                QString display = query.value(5).toString();
                model->insertRows(model->rowCount(), 1);
                QModelIndex index = model->index(model->rowCount()-1,0);
                model->setData(index, display);
                ui->treeView->setModel(model);
            }
        }
        query.clear();

        if(LoginDialog::Language == English)
        {
            for(int i = 0;i < scene_mgr_id.length();i++)
            {
                query.prepare("select * from translation where tablename like 'scene_mgr' and table_item_id = :table_item_id");   //翻译
                query.bindValue(":table_item_id",scene_mgr_id.at(i));
                if(query.exec())
                {
                    if(query.first())
                    {
                        if(query.value(4).toString() == "")
                        {
                            QString display = query.value(3).toString();
                            model->insertRows(model->rowCount(), 1);
                            QModelIndex index = model->index(model->rowCount()-1,0);
                            model->setData(index, display);
                            ui->treeView->setModel(model);
                        }
                        else
                        {
                            QString display = query.value(4).toString();
                            model->insertRows(model->rowCount(), 1);
                            QModelIndex index = model->index(model->rowCount()-1,0);
                            model->setData(index, display);
                            ui->treeView->setModel(model);
                        }
                    }
                }
            }
            query.clear();
        }
        for(int i = 0;i<scene_mgr_id.length();i++)                       //画面
        {
            query.prepare("select * from scene_mgr where node_type = 0 and parent_id = :parent_id order by pos asc");
            query.bindValue(":parent_id",scene_mgr_id.at(i));
            query.exec();
            while(query.next())
            {
                scene_in_use_id.append(query.value(3).toUInt());
                if(LoginDialog::Language == Chinese)
                {
                    QString display = query.value(5).toString();
                    scene_in_use_name.append(display);
                    QStandardItem* itemChild = new QStandardItem(display);
                    model->itemFromIndex(model->index(i,0))->appendRow(itemChild);
                    ui->treeView->setModel(model);
                }
                else if(LoginDialog::Language == English)
                {
                    QSqlQuery squery(db);
                    squery.prepare("select * from translation where tablename like 'scene_mgr' and table_item_id = :table_item_id");
                    squery.bindValue(":table_item_id",query.value(0));
                    if(squery.exec())
                    {
                        if(squery.first())
                        {
                            if(squery.value(4).toString() == "")
                            {
                                QString display = squery.value(3).toString();
                                QStandardItem* itemChild = new QStandardItem(display);
                                model->itemFromIndex(model->index(i,0))->appendRow(itemChild);
                                ui->treeView->setModel(model);
                                scene_in_use_name.append(display);
                            }
                            else
                            {
                                QString display = squery.value(4).toString();
                                QStandardItem* itemChild = new QStandardItem(display);
                                model->itemFromIndex(model->index(i,0))->appendRow(itemChild);
                                ui->treeView->setModel(model);
                                scene_in_use_name.append(display);
                            }
                        }
                    }
                }
            }
        }
        query.clear();
        //显示表初始化
        query.prepare("select scene_id from scene_mgr where scene_id IS NOT NULL");
        if(query.exec())
        {
            while(query.next())
            {
                if(!scene_zone_id_list.contains(query.value(0).toUInt()))
                {
                    scene_zone_id_list.append(query.value(0).toUInt());
                }
            }
        }
        query.clear();
        for(int i = 0;i < scene_zone_id_list.length(); i++)
        {
            query.prepare("select type from scene_zone where scene_zone_id = :scene_zone_id");
            query.bindValue(":scene_zone_id",QString::number(scene_zone_id_list.at(i)));
            if(query.exec())
            {
                while(query.next())
                {
                    scene_zone_type_list.append(query.value(0).toUInt());
                }
            }
        }
        query.clear();
        for(int i = 0;i < scene_zone_id_list.length(); i++)
        {
           uint x= 0;
           query.prepare("select signal_id from scene_items where scene_zone_id = :scene_zone_id ORDER BY pos ASC");
           query.bindValue(":scene_zone_id",scene_zone_id_list.at(i));
           if(query.exec())
           {
               while (query.next()) {
                   signal_id_list.append(query.value(0).toUInt());
                   x++;
               }
               scene_items_num_list.append(x);  //各个画面对应的信号数
               x = 0;
           }
        }
        query.clear();
        int cnt = 0;
        for(int i = 0; i <scene_zone_id_list.length(); i++)  //把用到的画面的信号数据放入列表中，结构体列表
        {
            if(!scene_items_num_list.isEmpty())
            {   
                for(int y = 0;y < scene_items_num_list.at(i); y++)
                {
                    Signal_Info signal_struct;
                    signal_struct.scene_zone_id = scene_zone_id_list.at(i);
                    signal_struct.scene_zone_type = scene_zone_type_list.at(i);
                    query.prepare("select * from signal where sign_id = :sign_id");
                    query.bindValue(":sign_id",signal_id_list.at(cnt));
                    cnt++;
                    if(query.exec())
                    {

                        while (query.next()) {
                            QSqlQuery squery(db);
                            if(LoginDialog::Language == Chinese)
                            {
                                signal_struct.Display = query.value(6).toString();
                                signal_struct.Value = 0;
                                signal_struct.Unit = query.value(8).toString();
                                signal_struct.address = query.value(4).toUInt();
                                signal_struct.data_type = query.value(7).toUInt();
                                signal_struct.factor = query.value(9).toFloat();
                                signal_struct.reg = query.value(3).toUInt();
                                signal_struct.offset = query.value(5).toUInt();
                                signal_struct.range = query.value(10).toString();
                                signal_struct.property = query.value(11).toString();
                                Signal_info_list.append(signal_struct);
                            }
                            else if (LoginDialog::Language == English) {
                                squery.prepare("select * from translation where tablename like 'signal' and table_item_id = :table_item_id");   //翻译
                                squery.bindValue(":table_item_id",query.value(0));
                                if(squery.exec())
                                {
                                    while (squery.next()) {
                                        if(squery.value(2).toString() == "display")
                                        {
                                            if(squery.value(4).toString() == "")   //英文翻译为空
                                            {
                                                signal_struct.Display = squery.value(3).toString();
                                            }
                                            else {
                                                signal_struct.Display = squery.value(4).toString();
                                            }
                                        }
                                        else if (squery.value(2).toString() == "property")
                                        {
                                            if(squery.value(4).toString() == "")   //英文翻译为空
                                            {
                                                signal_struct.property = squery.value(3).toString();
                                            }
                                            else {
                                                signal_struct.property = squery.value(4).toString();
                                            }
                                        }
                                    }
                                }
                                signal_struct.Value = 0;
                                signal_struct.Unit = query.value(8).toString();
                                signal_struct.address = query.value(4).toUInt();
                                signal_struct.data_type = query.value(7).toUInt();
                                signal_struct.factor = query.value(9).toFloat();
                                signal_struct.reg = query.value(3).toUInt();
                                signal_struct.offset = query.value(5).toUInt();
                                signal_struct.range = query.value(10).toString();
                                Signal_info_list.append(signal_struct);
                            }
                        }
                    }
                }
            }
        }
    }
}


void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    int scene_id = -1;
    QStandardItemModel* model = dynamic_cast<QStandardItemModel *>(ui->tableView->model());
    if(scene_in_use_name.indexOf(index.data().toString()) != -1)
        scene_id = scene_in_use_id.at(scene_in_use_name.indexOf(index.data().toString()));
    if(scene_id != -1)
    {
        for(int i = 0; i <=model->rowCount()+1; i++)
            model->removeRow(0);
        int pos = scene_zone_id_list.indexOf(scene_id);

        progressbar->setMinimum(0);
        progressbar->setMaximum(scene_items_num_list.at(pos));
        curr_progressbar_maxinum = scene_items_num_list.at(pos);
        for(int i = 0; i < scene_items_num_list.at(pos); i++)
        {
            Signal_Info signal_struct = Read_Signal_info_list(scene_id,i);
            model->insertRows(i, 1);
            for(int s = 0; s < 4; s++)
            {
                QModelIndex index = model->index(i,s);
                switch (s) {
                    case 0:
                        model->setData(index, signal_struct.Display);
                        break;
                    case 1:
                        model->setData(index, Value_Display_Process(signal_struct));   //值显示需要专用处理函数，因为有枚举类型
                        break;
                    case 2:
                        model->setData(index, signal_struct.Unit);
                        break;
                    case 3:
                        model->setData(index, signal_struct.range);
                        break;
                }

            }
        }
        ui->tableView->setModel(model);
        serial->Scene_pos = pos;
        serial->cnt = 0;
    }
    else{
        for(int i = 0; i <=model->rowCount()+1; i++)
            model->removeRow(0);
    }
}
void MainWindow::Data_Updata_Slot(int cnt)
{
    progressbar->setMaximum(curr_progressbar_maxinum);
    if(cnt == -1)  //超时断开
    {
        progressbar->setValue(0);
        progressbar->setFormat(tr("未连接"));
        serial->stopped = 1;
    }
    else if(cnt == -2)  //通信异常
    {
        progressbar->setMinimum(0);
        progressbar->setMaximum(0);
    }
    else if(cnt == -3)  //串口正常连接
    {
        progressbar->setValue(curr_progressbar_maxinum);
        progressbar->setFormat(tr("已连接"));
    }
    else if(cnt == -4)  //串口无法打开
    {
        progressbar->setValue(0);
        progressbar->setFormat(tr("未连接"));
        QMessageBox::warning(this,"Warning",tr("串口无法打开，请检查串口！"),QMessageBox::Yes);
        return;
    }
    else
    {
        progressbar->setFormat(tr("已连接"));
        int pos = 0;
        for(int i = 0; i < serial->Scene_pos; i++)
        {
            pos += scene_items_num_list[i];
        }
        progressbar->setValue(cnt);
        QStandardItemModel* model = dynamic_cast<QStandardItemModel *>(ui->tableView->model());
        QModelIndex index = model->index(cnt-1,1);
        model->setData(index, Value_Display_Process(Signal_info_list.at(pos+cnt-1)));  //显示部分需要专门的函数处理
        ui->tableView->setModel(model);
    }
}

void MainWindow::Property_Choice_Receive(QString s)
{
    QStringList slist = Signal_Info_Property(Clicked_Signal.property);
    QString vstr = slist.at(slist.indexOf(s)-1);
    switch (Clicked_Signal.data_type)
    {
        case 0:  //short
        {
            short value = vstr.toShort();
            float v = value/Clicked_Signal.factor;
            QString sNum = QString::number(v);
            value = sNum.toShort();
            Write_Value[0] = uchar((value>>8)&0xff);
            Write_Value[1] = uchar(value&0xff);
            serial->Write_FLAG = 1;
        }
        break;
        case 1:  //ushort
        {
            ushort value = vstr.toUShort();
            float v = value/Clicked_Signal.factor;
            QString sNum = QString::number(v);
            value = sNum.toUShort();
            Write_Value[0] = uchar((value>>8)&0xff);
            Write_Value[1] = uchar(value&0xff);
            serial->Write_FLAG = 1;
        }
        break;
        case 2:  //int(abcd)
        case 3:  //int(cdab)
        case 4:  //int(badc)
        case 5:  //int(dcba)
        {
            int value = vstr.toInt();
            float v = value/Clicked_Signal.factor;
            QString sNum = QString::number(v);
            value = sNum.toInt();
            Write_Value[0] = uchar((value>>24)&0xff);
            Write_Value[1] = uchar((value>>16)&0xff);
            Write_Value[2] = uchar((value>>8)&0xff);
            Write_Value[3] = uchar(value&0xff);
            serial->Write_FLAG = 1;
        }
        break;
        case 6:  //uint(abcd)
        case 7:  //uint(cdab)
        case 8:  //uint(badc)
        case 9:  //uint(dcba)
        {
            uint value = vstr.toUInt();
            float v = value/Clicked_Signal.factor;
            QString sNum = QString::number(v);
            value = sNum.toUInt();
            Write_Value[0] = uchar((value>>24)&0xff);
            Write_Value[1] = uchar((value>>16)&0xff);
            Write_Value[2] = uchar((value>>8)&0xff);
            Write_Value[3] = uchar(value&0xff);
            serial->Write_FLAG = 1;
        }
        break;
        case 10:  //float(abcd)
        case 11:  //float(cdab)
        case 12:  //float(badc)
        case 13:  //float(dcba)
        {
            float value = vstr.toFloat();
            value = value/Clicked_Signal.factor;
            QString sNum = QString::number(value);
            int v = sNum.toInt();
            Write_Value[0] = uchar((v>>24)&0xff);
            Write_Value[1] = uchar((v>>16)&0xff);
            Write_Value[2] = uchar((v>>8)&0xff);
            Write_Value[3] = uchar(v&0xff);
            serial->Write_FLAG = 1;
        }
        break;
        case 14:   //string
        {
            Write_Value = vstr.toLatin1();
            serial->Write_FLAG = 1;
        }
        break;

    }
}

QStringList MainWindow::Signal_Info_Property(QString s)
{
    QStringList list_enum;
    if(s != "")
    {
        QString str = s;
        str = str.mid(1,str.length()-7);
        QStringList list = str.split(",");
        for(int i = 0; i < list.length(); i++)
        {
            QStringList list_t = list[i].split(":");
            list_enum.append(list_t);
        }
       return list_enum;
    }
}

Signal_Info MainWindow::Read_Signal_info_list(int scene_zone_id,int item_pos)
{
    int real_pos = 0;
    int pos = scene_zone_id_list.indexOf(scene_zone_id);
    for(int i = 0; i < pos; i++)
    {
        real_pos += scene_items_num_list.at(i);
    }
    real_pos += item_pos;
    if(real_pos > Signal_info_list.length())
        qDebug()<<"real_pos"<<real_pos;
    return Signal_info_list.at(real_pos);
}

void MainWindow::on_RunAction_triggered()
{
    if(com == "")
    {
        QMessageBox::warning(this,"Warning",tr("请先进行通讯设置"),QMessageBox::Yes);
        return;
    }
    else
    {

        emit Doworks_Signal();
    }
}

void MainWindow::on_StopAction_triggered()   //停止线程
{
    serial->stopped = 1;
    emit Stop_Thread_Signal();
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)    //点击数据表(可设置表格有效)
{
    int minvalue;
    int maxvalue;
    bool ok;
    if(index.column() == 1)  //点击显示列有效
    {
       Signal_Info signal_struct = Read_Signal_info_list(scene_zone_id_list.at(serial->Scene_pos),index.row());
       Clicked_Signal = signal_struct;
       if(signal_struct.scene_zone_type == 0)  //设置界面
       {
           Write_Value.clear();
           if(signal_struct.range != "")
           {
               minvalue = signal_struct.range.left(signal_struct.range.indexOf("~")).toInt();
               maxvalue = signal_struct.range.right(signal_struct.range.length() - signal_struct.range.indexOf("~")-1).toInt();
           }
           if(signal_struct.property == "")
           {
               switch (signal_struct.data_type)
               {
                   case 0:  //short
                   {
                       short value;
                       if(signal_struct.range != "")
                       {
                           value = QInputDialog::getInt(this,tr("Input Dialog"),tr("请输入："),0,minvalue,maxvalue,1,&ok);
                       }
                       else
                       {
                           value = QInputDialog::getInt(this,tr("Input Dialog"),tr("请输入："),0,-32767,32767,1,&ok);
                       }
                       float v = value/signal_struct.factor;
                       QString sNum = QString::number(v);
                       value = sNum.toShort();
                       if(ok)
                       {
                           Write_Value[0] = uchar((value>>8)&0xff);
                           Write_Value[1] = uchar(value&0xff);
                           serial->Write_FLAG = 1;
                       }

                   }
                   break;
                   case 1:  //ushort
                   {
                       ushort value;
                       if(signal_struct.range != "")
                       {
                           value = QInputDialog::getInt(this,tr("Input Dialog"),tr("请输入："),0,minvalue,maxvalue,1,&ok);
                       }
                       else
                       {
                           value = QInputDialog::getInt(this,tr("Input Dialog"),tr("请输入："),0,0,65535,1,&ok);
                       }
                       float v = value/signal_struct.factor;
                       QString sNum = QString::number(v);
                       value = sNum.toUShort();
                       if(ok)
                       {
                           Write_Value[0] = uchar((value>>8)&0xff);
                           Write_Value[1] = uchar(value&0xff);
                           serial->Write_FLAG = 1;
                       }

                   }
                   break;
                   case 2:  //int(abcd)
                   case 3:  //int(cdab)
                   case 4:  //int(badc)
                   case 5:  //int(dcba)
                   {
                       int value;
                       if(signal_struct.range != "")
                       {
                           value = QInputDialog::getInt(this,tr("Input Dialog"),tr("请输入："),0,minvalue,maxvalue,1,&ok);
                       }
                       else
                       {
                           value = QInputDialog::getInt(this,tr("Input Dialog"),tr("请输入："),0,-2147483647,2147483647,1,&ok);
                       }
                       float v = value/signal_struct.factor;
                       QString sNum = QString::number(v);
                       value = sNum.toInt();
                       if(ok)
                       {
                           Write_Value[0] = uchar((value>>24)&0xff);
                           Write_Value[1] = uchar((value>>16)&0xff);
                           Write_Value[2] = uchar((value>>8)&0xff);
                           Write_Value[3] = uchar(value&0xff);
                           serial->Write_FLAG = 1;
                       }

                   }
                   break;
                   case 6:  //uint(abcd)
                   case 7:  //uint(cdab)
                   case 8:  //uint(badc)
                   case 9:  //uint(dcba)
                   {
                       uint value;
                       if(signal_struct.range != "")
                       {
                           value = QInputDialog::getInt(this,tr("Input Dialog"),tr("请输入："),0,minvalue,maxvalue,1,&ok);
                       }
                       else
                       {
                           value = QInputDialog::getInt(this,tr("Input Dialog"),tr("请输入："),0,-2147483647,2147483647,1,&ok);
                       }
                       float v = value/signal_struct.factor;
                       QString sNum = QString::number(v);
                       value = sNum.toUInt();
                       if(ok)
                       {
                           Write_Value[0] = uchar((value>>24)&0xff);
                           Write_Value[1] = uchar((value>>16)&0xff);
                           Write_Value[2] = uchar((value>>8)&0xff);
                           Write_Value[3] = uchar(value&0xff);
                           serial->Write_FLAG = 1;
                       }

                   }
                   break;
                   case 10:  //float(abcd)
                   case 11:  //float(cdab)
                   case 12:  //float(badc)
                   case 13:  //float(dcba)
                   {
                       float value;
                       if(signal_struct.range != "")
                       {
                           value = QInputDialog::getDouble(this,tr("Input Dialog"),tr("请输入："),0,minvalue,maxvalue,1,&ok);
                       }
                       else
                       {
                           value = QInputDialog::getDouble(this,tr("Input Dialog"),tr("请输入："),0,-2147483647,2147483647,1,&ok);
                       }
                       value = value/signal_struct.factor;
                       QString sNum = QString::number(value);
                       int v = sNum.toInt();
                       if(ok)
                       {
                           Write_Value[0] = uchar((v>>24)&0xff);
                           Write_Value[1] = uchar((v>>16)&0xff);
                           Write_Value[2] = uchar((v>>8)&0xff);
                           Write_Value[3] = uchar(v&0xff);
                           serial->Write_FLAG = 1;
                       }

                   }
                   break;
                   case 14:   //string
                   {
                       QString value = QInputDialog::getText(this,tr("Input Dialog"),tr("请输入："),QLineEdit::Normal,tr("长度为:%1").arg(signal_struct.offset),&ok);
                       if(ok)
                       {
                            Write_Value = value.toLatin1();
                            serial->Write_FLAG = 1;
                       }
                   }
                   break;

               }
           }
           else
           {
               ChooseEnumItemDialog *w = new ChooseEnumItemDialog(this);
               connect(w,&ChooseEnumItemDialog::Send_Property_Choice,this,&MainWindow::Property_Choice_Receive);
               w->exec();
           }
       }
    }
}

QVariant MainWindow::Value_Display_Process(Signal_Info s)
{
    QStringList list_enum;
    if(s.property != "")
    {      
        QString str = s.property;
        str = str.mid(1,str.length()-7);
        QStringList list = str.split(",");
        for(int i = 0; i < list.length(); i++)
        {
            QStringList list_t = list[i].split(":");
            list_enum.append(list_t);
        }
       int pos = list_enum.indexOf(s.Value.toString());
       return list_enum.at(pos+1);
    }
    return s.Value;
}
