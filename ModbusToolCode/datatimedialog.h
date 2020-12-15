#ifndef DATATIMEDIALOG_H
#define DATATIMEDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QDateTime>
namespace Ui {
class DataTimeDialog;
}

class DataTimeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DataTimeDialog(QWidget *parent = nullptr);
    ~DataTimeDialog();
signals:
    void Send_DataTime_Signal(QDateTime);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::DataTimeDialog *ui;
};

#endif // DATATIMEDIALOG_H
