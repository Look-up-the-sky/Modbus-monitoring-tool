#include "scenemgrdialog.h"
#include "ui_scenemgrdialog.h"
#include "mainwindow.h"


SceneMgrDialog::SceneMgrDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SceneMgrDialog)
{
    ui->setupUi(this);
    TableInit();
    TableBindDb(MainWindow::DbName);
}

SceneMgrDialog::~SceneMgrDialog()
{
    delete ui;
}

void SceneMgrDialog::TableInit()
{
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    ui->tableView->horizontalHeader()->sortIndicatorOrder();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setSectionsClickable(true);
    ui->tableView->horizontalHeader()->setHighlightSections(false);
    ui->tableView->horizontalHeader()->setBackgroundRole(QPalette::Background);

}

void SceneMgrDialog::TableBindDb(QString DB)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./Config/"+DB+".db");
    if(QFile::exists("./Config/"+DB+".db"))
    {
        if(db.open())
        {
            QSqlTableModel *pModel = new QSqlTableModel(this,db);
            pModel->setTable("scene_zone");
            pModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
            pModel->select();
            pModel->setHeaderData(3, Qt::Horizontal, tr("选择画面"));
            pModel->sort(0, Qt::AscendingOrder);
            ui->tableView->setModel(pModel);
            ui->tableView->hideColumn(0);  //隐藏前三列
            ui->tableView->hideColumn(1);
            ui->tableView->hideColumn(2);
            ui->tableView->setColumnWidth(3,378);  //列宽度与tableview相同

        }

    }
}

void SceneMgrDialog::on_pushButton_clicked()  //OK
{
    QString str = ui->tableView->currentIndex().data().toString();
    if(str == "")
    {
        QMessageBox::warning(this,"Warning",tr("未选择任何画面！"),QMessageBox::Yes);
        return;
    }
    emit Send_Scene_Choose_Info(str);
    this->close();
}

void SceneMgrDialog::on_pushButton_2_clicked()  //Cancel
{
    this->close();
}
