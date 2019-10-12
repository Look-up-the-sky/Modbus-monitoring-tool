#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QDir>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    void GetAllDbName();
    QStringList string_list;
    static uint Language;
signals:
    void DbNameSignal(QString);

private slots:
    void on_pushButton_2_clicked();
private:
    Ui::LoginDialog *ui;
    MainWindow *MainWindow_k;
};

#endif // LOGINDIALOG_H
