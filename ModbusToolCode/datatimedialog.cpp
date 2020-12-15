#include "datatimedialog.h"
#include "ui_datatimedialog.h"

DataTimeDialog::DataTimeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataTimeDialog)
{
    ui->setupUi(this);
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
}

DataTimeDialog::~DataTimeDialog()
{
    delete ui;
}


void DataTimeDialog::on_pushButton_clicked()    //ok
{
    emit Send_DataTime_Signal(ui->dateTimeEdit->dateTime());
    this->close();

}

void DataTimeDialog::on_pushButton_2_clicked()   //cancel
{
    this->close();
}

