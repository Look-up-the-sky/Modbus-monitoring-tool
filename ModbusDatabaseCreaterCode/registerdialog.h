#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QTranslator>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog() override;
    void closeEvent(QCloseEvent *) override;
    QTranslator translator;
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();
signals:
    void Send_Close_Signal();
protected:
    QFileInfo Filename;
    void creatDb();

private:
    Ui::RegisterDialog *ui;
};

#endif // REGISTERDIALOG_H
