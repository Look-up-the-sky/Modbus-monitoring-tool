#include "chooseenumitemdialog.h"
#include "ui_chooseenumitemdialog.h"
#include "mainwindow.h"


ChooseEnumItemDialog::ChooseEnumItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseEnumItemDialog)
{
    ui->setupUi(this);
    Signal_Info_Property_List = Signal_Info_Property(MainWindow::Clicked_Signal.property);
    QStringList s;
    for(int i = 0; i < Signal_Info_Property_List.length(); i++)
    {
        if((i%2) != 0)
            s.append(Signal_Info_Property_List.at(i));
    }
    ui->comboBox->addItems(s);

}

ChooseEnumItemDialog::~ChooseEnumItemDialog()
{
    delete ui;
}

QStringList ChooseEnumItemDialog::Signal_Info_Property(QString s)
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

void ChooseEnumItemDialog::on_pushButton_clicked()  //ok
{
    MainWindow::Clicked_Signal.Value = ui->comboBox->currentText();
    emit Send_Property_Choice(ui->comboBox->currentText());
    this->close();
}

void ChooseEnumItemDialog::on_pushButton_2_clicked() //cancel
{
    this->close();
}
