#ifndef COMMSETDIALOG_H
#define COMMSETDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

struct CommSetInfo
{
    uint addr;
    uint outtime;
    QString com;
    int Baud;
    uint date_bit;
    QString Check_bit;
    QString Stop_bit;
};
Q_DECLARE_METATYPE(CommSetInfo)
namespace Ui {
class CommSetDialog;
}

class CommSetDialog : public QDialog
{
    Q_OBJECT
signals:
    void Send_Comm_Set_Info(QVariant&);
public:
    explicit CommSetDialog(QWidget *parent = nullptr);
    ~CommSetDialog();
    CommSetInfo comsetinfo;
    QString nameSerialport[3];
    void scan_serial_port();
    void time_handle();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::CommSetDialog *ui;
};

#endif // COMMSETDIALOG_H
