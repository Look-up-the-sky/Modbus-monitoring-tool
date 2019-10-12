#include "addscenezonedialog.h"
#include "ui_addscenezonedialog.h"

AddSceneZoneDialog::AddSceneZoneDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddSceneZoneDialog)
{
    ui->setupUi(this);
}

AddSceneZoneDialog::~AddSceneZoneDialog()
{
    delete ui;
}

void AddSceneZoneDialog::on_listWidget_currentRowChanged(int currentRow)
{
    ui->label->setText(ui->listWidget->currentIndex().data().toString());
    ui->lineEdit->setText(tr("新建")+ui->listWidget->currentIndex().data().toString());
}

void AddSceneZoneDialog::on_pushButton_clicked()  //OK
{
    QVariant variant;
    addscenezoneinfo.type = ui->listWidget->currentRow();
    addscenezoneinfo.label = ui->lineEdit_2->text();
    addscenezoneinfo.display = ui->lineEdit->text();
    variant.setValue(addscenezoneinfo);
    emit Send_AddSceneZoneInfo(variant);
    this->close();
}

void AddSceneZoneDialog::on_pushButton_2_clicked()  //Cancel
{
    this->close();
}
