#include "addsceneitemdialog.h"
#include "ui_addsceneitemdialog.h"
#include "mainwindow.h"
QString MainWindow::DbName = "";


AddSceneItemDialog::AddSceneItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddSceneItemDialog)
{
    ui->setupUi(this);
    TableInit();
    TableBindDb(MainWindow::DbName);
}

AddSceneItemDialog::~AddSceneItemDialog()
{
    delete ui;
}

void AddSceneItemDialog::TableInit()
{
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    ui->tableView->horizontalHeader()->sortIndicatorOrder();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setSectionsClickable(true);
    ui->tableView->horizontalHeader()->setHighlightSections(false);
    ui->tableView->horizontalHeader()->setBackgroundRole(QPalette::Background);

    ui->tableView_2->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->setSortingEnabled(true);
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    ui->tableView_2->horizontalHeader()->sortIndicatorOrder();
    ui->tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_2->verticalHeader()->hide();
    ui->tableView_2->horizontalHeader()->setSectionsClickable(true);
    ui->tableView_2->horizontalHeader()->setHighlightSections(false);
    ui->tableView_2->horizontalHeader()->setBackgroundRole(QPalette::Background);
}

void AddSceneItemDialog::TableBindDb(QString DB)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./Config/"+DB+".db");
    if(QFile::exists("./Config/"+DB+".db"))
    {
        if(db.open())
        {
            QSqlTableModel *pModel1 = new QSqlTableModel(this,db);
            pModel1->setTable("signal_zone");
            pModel1->setEditStrategy(QSqlTableModel::OnManualSubmit);
            pModel1->select();
            pModel1->setHeaderData(1, Qt::Horizontal, tr("信号分组"));
            pModel1->sort(0, Qt::AscendingOrder);
            ui->tableView->setModel(pModel1);
            ui->tableView->hideColumn(0);  //隐藏第一列

            QSqlTableModel *pModel2 = new QSqlTableModel(this,db);
            pModel2->setTable("signal");
            pModel2->setEditStrategy(QSqlTableModel::OnManualSubmit);
            pModel2->select();
            pModel2->sort(0, Qt::AscendingOrder);
            ui->tableView_2->setModel(pModel2);
        }

    }
}

void AddSceneItemDialog::on_tableView_clicked(const QModelIndex &index)
{
    QSqlTableModel *Mode = dynamic_cast<QSqlTableModel *>(ui->tableView_2->model());
    Mode->setFilter("sign_zone_id = "+QString::number(index.row()));
}

void AddSceneItemDialog::on_pushButton_clicked()   //OK
{
    QItemSelectionModel *selectionModel =ui->tableView_2->selectionModel();
    QModelIndexList indexes = selectionModel->selectedIndexes();
    QString str;
    str = str +"{";
    foreach(QModelIndex index,indexes)
    {
        if(index.column() == 0)
            str = str + "(" + index.data().toString() + ",";
        if(index.column() == 6)
            str = str + index.data().toString() + ")";
    }
    str = str +"}";
    emit Send_AddSceneItemsInfo(str);
    this->close();
}

void AddSceneItemDialog::on_pushButton_2_clicked()  //Cancel
{
    this->close();
}
