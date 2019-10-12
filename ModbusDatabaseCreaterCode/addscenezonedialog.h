#ifndef ADDSCENEZONEDIALOG_H
#define ADDSCENEZONEDIALOG_H

#include <QDialog>

struct AddSceneZoneInfo
{
    int type;
    QString label;
    QString display;
};
Q_DECLARE_METATYPE(AddSceneZoneInfo)
namespace Ui {
class AddSceneZoneDialog;
}

class AddSceneZoneDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddSceneZoneDialog(QWidget *parent = nullptr);
    ~AddSceneZoneDialog();
    AddSceneZoneInfo addscenezoneinfo;
signals:
    void Send_AddSceneZoneInfo(QVariant&);
private slots:
    void on_listWidget_currentRowChanged(int currentRow);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::AddSceneZoneDialog *ui;
};

#endif // ADDSCENEZONEDIALOG_H
