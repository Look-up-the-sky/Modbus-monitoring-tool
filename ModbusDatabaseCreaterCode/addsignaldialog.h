#ifndef ADDSIGNALDIALOG_H
#define ADDSIGNALDIALOG_H

#include <QDialog>
#include <QVariant>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDebug>
struct AddInfo
{
    int sign_id;
    int sign_zone_id;
    QString sign_zone_name;
    int reg;
    int addr;
    int offset;
    QString display;
    int data_type;
    QString uint;
    QString factor;
    QString range;
    QString property;

};
Q_DECLARE_METATYPE(AddInfo)

namespace Ui {
class AddSignalDialog;
}

class AddSignalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddSignalDialog(QWidget *parent = nullptr);
    ~AddSignalDialog();
    QStringList Signal_Info_Property(QString s);
    void Copy();
    void Paste();
protected:
    virtual void keyPressEvent(QKeyEvent *event) override;

signals:
    void AddInfoSignal(QVariant&);
public slots:
    void Modify_Signal_Info_Receive(QVariant v);
private:
    Ui::AddSignalDialog *ui;

public:
    AddInfo addinfo;
private slots:
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_comboBox_datatype_activated(const QString &arg1);
    void on_pushButton_add_clicked();
    void on_pushButton_del_clicked();
};

#endif // ADDSIGNALDIALOG_H
