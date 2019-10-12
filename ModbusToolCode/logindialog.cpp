#include "logindialog.h"
#include "ui_logindialog.h"


LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    GetAllDbName();

}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_pushButton_2_clicked()   //登录按钮
{
    if((ui->comboBox->currentText() == "")||(ui->lineEdit->text() == "")
            ||(ui->lineEdit_2->text() == ""))
    {
        QMessageBox::warning(this,"Warning",tr("必要选项未填！"),QMessageBox::Yes);
        return;
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");           //验证登录密码
    db.setDatabaseName("./Config/"+ui->comboBox->currentText()+".db");
    if(!string_list.contains(ui->comboBox->currentText()))
    {
        QMessageBox::warning(this,"Warning",tr("无此数据库！"),QMessageBox::Yes);
        return;
    }
    if(db.open())
    {
        QSqlQuery query(db);
        query.prepare("select * from users where UserName = :UserName and Password = :Password");
        query.bindValue(":UserName",ui->lineEdit->text());   // 绑定数据到指定的位置
        query.bindValue(":Password",ui->lineEdit_2->text());
        if(query.exec())
        {
            QSqlRecord rec = query.record();
            if(query.first())
            {
                rec = query.record();
                int UserNamecol = rec.indexOf("UserName");
                qDebug()<<"UserName:"<<query.value(UserNamecol).toString();
            }
            else
            {
                QMessageBox::warning(this,"Warning",tr("用户名或密码错误！"),QMessageBox::Yes);
                return;
            }
        }
        query.clear();
        query.prepare("select * from translation");     //翻译表为空
        if(query.exec())
        {
            if(query.next())
            {
                if(ui->comboBox_2->currentText() == "Chinese")
                {
                    Language = 0;
                }
                else if (ui->comboBox_2->currentText() == "English")
                {
                    Language = 1;
                }
                else
                {
                    Language = 0;
                }

            }
            else
            {
                Language = 0;
            }
        }
        db.close();
    }
    MainWindow_k = new MainWindow;
    connect(MainWindow_k,&MainWindow::CloseSignal,this,&LoginDialog::show);
    connect(this,&LoginDialog::DbNameSignal,MainWindow_k,&MainWindow::DbNameSlot);
    emit DbNameSignal(ui->comboBox->currentText());
    MainWindow_k->show();
    this->hide();

}


void LoginDialog::GetAllDbName()
{
    //判断路径是否存在
        QDir dir("./Config");   //程序目录下的Config文件夹，固定地址
        if(!dir.exists())
        {
            QMessageBox::warning(this,"Warning",tr("找不到配置文件夹！"),QMessageBox::Yes);
            return;
        }


        //查看路径中后缀为.db格式的文件
        QStringList filters;
        filters<<QString("*.db");
        dir.setFilter(QDir::Files | QDir::NoSymLinks); //设置类型过滤器，只为文件格式
        dir.setNameFilters(filters);  //设置文件名称过滤器，只为filters格式

        //统计db格式的文件个数
        int dir_count = dir.count();
        if(dir_count <= 0)
        {
             QMessageBox::warning(this,"Warning",tr("配置文件夹为空！"),QMessageBox::Yes);
            return;
        }


        //存储文件名称
        for(int i=0; i<dir_count; i++)
        {
            QString file_name = dir[i];  //文件名称
            file_name = file_name.left(file_name.length()-3);
            string_list.append(file_name);

        }
        ui->comboBox->clear();
        ui->comboBox->addItems(string_list);
}


