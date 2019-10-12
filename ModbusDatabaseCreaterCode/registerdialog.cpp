#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "logindialog.h"

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    if(LoginDialog::Language == 1)
    {
        translator.load(":/Language/C_I18N_en_UK.qm");
        QApplication::instance()->installTranslator(&translator);
        ui->retranslateUi(this);
    }
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::closeEvent(QCloseEvent *)
{
    emit Send_Close_Signal();
    this->deleteLater();
}

void RegisterDialog::on_pushButton_clicked()
{
    QDir dir;
    if(!dir.exists("./Config"))
    {
        dir.mkpath("./Config");
    }
    Filename = QFileDialog::getSaveFileName(this,tr("保存"),"./Config/"+ui->lineEdit->text(),tr("Sqlite(*.db)"));
    ui->textEdit->setText(Filename.filePath());
    ui->lineEdit->setText(Filename.fileName());


}

void RegisterDialog::creatDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(Filename.filePath());

    if(db.open())
    {
        QString sqlstr="create table users(UserName TEXT not null,Password TEXT not null)"; //用户表
        QSqlQuery query(db);
        query.exec(sqlstr);
        query.prepare("insert into users (UserName, Password) values(:UserName, :Password)");
        query.bindValue(":UserName", QString::fromLocal8Bit("admin"));
        query.bindValue(":Password", "admin");
        query.exec();
        query.clear();
        query.prepare("insert into users (UserName, Password) values(:UserName, :Password)");
        query.bindValue(":UserName", ui->lineEdit_2->text());
        query.bindValue(":Password", ui->lineEdit_3->text());
        query.exec();
        query.clear();
        query.exec("create table signal_zone("                           //信号组表
                   "sign_zone_id INTEGER PRIMARY KEY AUTOINCREMENT,"     //表示该列为整数递增,如果为空时则自动填入1,然后在下面的每一行都会自动+1, PRIMARY KEY则表示该列作为列表的主键,通过它可以轻易地获取某一行数据
                   "sign_zone_name TEXT not null)"
                    );
        query.exec("create table signal("                                //信号表
                   "sign_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "sign_zone_id INTEGER,"
                   "sign_zone_name TEXT,"
                   "reg INTEGER,"
                   "addr INTEGER,"
                   "offset INTEGER,"
                   "display TEXT,"
                   "data_type INTEGER,"
                   "unit TEXT,"
                   "factor REAL,"
                   "range TEXT,"
                   "property TEXT)"
                    );
        query.exec("create table scene_zone("                           //画面表
                   "scene_zone_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "type INTEGER,"
                   "label TEXT,"
                   "display TEXT)"
                    );
        query.exec("create table scene_items("                           //画面内容表
                   "scene_items_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "scene_zone_id INTEGER,"
                   "type TEXT,"
                   "display TEXT,"
                   "signal_id INTEGER,"
                   "pos INTEGER,"
                   "widget_type INTEGER,"
                   "enable INTEGER)"
                    );
        query.exec("create table scene_mgr("                           //画面管理表
                   "scene_mgr_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "node_type INTEGER,"
                   "parent_id INTEGER,"
                   "scene_id INTEGER,"
                   "pos INTEGER,"
                   "display TEXT)"
                    );
        query.exec("create table translation("                           //翻译表
                   "tablename TEXT,"
                   "table_item_id INTEGER,"
                   "table_field TEXT,"
                   "src TEXT,"
                   "english TEXT)"
                    );

    }
    db.close();
}

void RegisterDialog::on_pushButton_2_clicked()  //OK
{
    if((ui->lineEdit_2->text() == "")||(ui->lineEdit_3->text() == "")
            ||(ui->lineEdit->text() == "")||(Filename.filePath().isEmpty()))
    {
        QMessageBox::warning(this,"Warning",tr("必要选项未填，数据库无法创建！"),QMessageBox::Yes);
        return;
    }
    creatDb();
    this->close();
}

void RegisterDialog::on_pushButton_3_clicked()   //Cancel
{

    this->close();
}
