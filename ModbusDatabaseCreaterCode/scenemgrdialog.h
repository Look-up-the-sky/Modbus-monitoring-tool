#ifndef SCENEMGRDIALOG_H
#define SCENEMGRDIALOG_H

#include <QDialog>
#include <QSqlDatabase>


namespace Ui {
class SceneMgrDialog;
}

class SceneMgrDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SceneMgrDialog(QWidget *parent = nullptr);
    ~SceneMgrDialog();
public:
    void TableInit();
    void TableBindDb(QString DB);
    QSqlDatabase db;
signals:
    void Send_Scene_Choose_Info(QString&);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::SceneMgrDialog *ui;
};

#endif // SCENEMGRDIALOG_H
