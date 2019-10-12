#include "commsetdialog.h"
#include "ui_commsetdialog.h"
#include <QTimer>

CommSetDialog::CommSetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommSetDialog)
{
    ui->setupUi(this);
    QTimer *M_time = new QTimer(this);
    connect(M_time,&QTimer::timeout,this,&CommSetDialog::time_handle);
    M_time->start(1000);
}

CommSetDialog::~CommSetDialog()
{
    delete ui;
}

void CommSetDialog::on_pushButton_clicked()
{
    comsetinfo.addr = ui->lineEdit->text().toUInt();
    comsetinfo.outtime = ui->lineEdit_2->text().toUInt();
    comsetinfo.com = ui->comboBox->currentText();
    comsetinfo.Baud = ui->comboBox_2->currentText().toUInt();
    comsetinfo.date_bit = ui->comboBox_3->currentText().toUInt();
    comsetinfo.Check_bit = ui->comboBox_4->currentText();
    comsetinfo.Stop_bit = ui->comboBox_5->currentText();
    QVariant variant;
    variant.setValue(comsetinfo);
    emit Send_Comm_Set_Info(variant);
    this->close();
}

void CommSetDialog::on_pushButton_2_clicked()
{
    this->close();
}

void CommSetDialog::scan_serial_port()
{
    int Port_count = 0;
    ui->comboBox->setItemText(0,tr("None"));
    ui->comboBox->setItemText(1,"");
    ui->comboBox->setItemText(2,"");
    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
        {
            nameSerialport[Port_count] = info.portName();
            ui->comboBox->setItemText(Port_count,nameSerialport[Port_count]);
            Port_count++;
        }
}

void CommSetDialog::time_handle()
{
    scan_serial_port();
}
