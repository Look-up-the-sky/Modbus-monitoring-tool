#ifndef ADDSCENEITEMDIALOG_H
#define ADDSCENEITEMDIALOG_H

#include <QDialog>
#include <QSqlDatabase>


namespace Ui {
class AddSceneItemDialog;
}

class AddSceneItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddSceneItemDialog(QWidget *parent = nullptr);
    ~AddSceneItemDialog();
    void TableInit();
    void TableBindDb(QString DB);
    QSqlDatabase db;
signals:
    void Send_AddSceneItemsInfo(QString&);
private slots:
    void on_tableView_clicked(const QModelIndex &index);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::AddSceneItemDialog *ui;
};

#endif // ADDSCENEITEMDIALOG_H
