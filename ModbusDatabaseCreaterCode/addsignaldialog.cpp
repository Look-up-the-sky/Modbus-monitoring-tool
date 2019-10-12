#include "addsignaldialog.h"
#include "ui_addsignaldialog.h"

AddSignalDialog::AddSignalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddSignalDialog)
{
    ui->setupUi(this);
    QValidator *validator1 =new QIntValidator(this);
    ui->lineEdit_min->setValidator(validator1);
    QValidator *validator2 =new QIntValidator(this);
    ui->lineEdit_max->setValidator(validator2);
    QRegExp reg3("^[0-9]*[1-9][0-9]*$");
    QValidator *validator3 = new QRegExpValidator(reg3, ui->lineEdit_addr );
    ui->lineEdit_addr->setValidator(validator3);
}

AddSignalDialog::~AddSignalDialog()
{
    delete ui;
}

void AddSignalDialog::Modify_Signal_Info_Receive(QVariant v)
{
    AddInfo addinfo;
    addinfo = v.value<AddInfo>();
    ui->comboBox_reg->setCurrentText(QString::number(addinfo.reg));
    ui->lineEdit_addr->setText(QString::number(addinfo.addr));
    if(addinfo.data_type == 14)
    {
        ui->lineEdit_offset->setEnabled(true);
        ui->lineEdit_offset->setText(QString::number(addinfo.offset));
    }
    ui->comboBox_datatype->setCurrentIndex(addinfo.data_type);
    ui->lineEdit_miaoshu->setText(addinfo.display);
    ui->lineEdit_unit->setText(addinfo.uint);
    ui->comboBox_xishu->setCurrentText(addinfo.factor);
    if(addinfo.range != "")
    {
        int pos = addinfo.range.indexOf("~");
        ui->lineEdit_min->setText(addinfo.range.left(pos));
        ui->lineEdit_max->setText(addinfo.range.right(addinfo.range.length()-pos-1));
    }
    if(addinfo.property != "")
    {
        QStringList list = Signal_Info_Property(addinfo.property);
        int len = list.length()/2;
        for(int i = 0; i < len; i++)
        {
            ui->tableWidget_enum->model()->insertRow(i);
            ui->tableWidget_enum->model()->setData(ui->tableWidget_enum->model()->index(i,0),list.at(i*2));
            ui->tableWidget_enum->model()->setData(ui->tableWidget_enum->model()->index(i,1),list.at(i*2+1));
        }
    }

}

QStringList AddSignalDialog::Signal_Info_Property(QString s)
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

void AddSignalDialog::on_pushButton_3_clicked()  //OK
{
    if(ui->lineEdit_miaoshu->text().isEmpty()||ui->lineEdit_addr->text().isEmpty())
    {
        QMessageBox::warning(this,"Warning",tr("必要参数未填！"),QMessageBox::Yes);
        return;
    }
    QVariant variant;
    addinfo.reg = ui->comboBox_reg->currentText().toInt();
    addinfo.addr = ui->lineEdit_addr->text().toInt();
    addinfo.offset = ui->lineEdit_offset->text().toInt();
    addinfo.display = ui->lineEdit_miaoshu->text();
    addinfo.data_type = ui->comboBox_datatype->currentIndex();
    addinfo.uint = ui->lineEdit_unit->text();
    addinfo.factor = ui->comboBox_xishu->currentText();  //字符串形式，避免小数点太长
    if(ui->lineEdit_min->text().isEmpty()||ui->lineEdit_max->text().isEmpty())
    {
        addinfo.range = "";
    }
    else
    {
        addinfo.range = ui->lineEdit_min->text()+"~"+ui->lineEdit_max->text();
    }
    QAbstractItemModel *model = ui->tableWidget_enum->model();
    if(ui->tableWidget_enum->model()->rowCount() != 0)
    {
        QString property_str = "{";
        for(int i = 0;i < model->rowCount();i++)
        {
            property_str = property_str + model->data(model->index(i,0)).toString()+":";
            property_str = property_str +  model->data(model->index(i,1)).toString()+",";
        }
        property_str = property_str+"enum}";
        addinfo.property = property_str;
    }
    variant.setValue(addinfo);
    emit AddInfoSignal(variant);
    this->close();
}

void AddSignalDialog::on_pushButton_4_clicked() //Cancel
{
    this->close();
}

void AddSignalDialog::on_comboBox_datatype_activated(const QString &arg1)
{
    if(arg1 == "string")
    {
       ui->lineEdit_offset->setEnabled(true);
    }
    else
    {
       ui->lineEdit_offset->setDisabled(true);
    }
}

void AddSignalDialog::on_pushButton_add_clicked()
{
    ui->tableWidget_enum->model()->insertRow(ui->tableWidget_enum->model()->rowCount());
}

void AddSignalDialog::on_pushButton_del_clicked()
{
    ui->tableWidget_enum->model()->removeRow(ui->tableWidget_enum->currentRow());
}
