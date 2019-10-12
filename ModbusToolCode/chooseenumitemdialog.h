#ifndef CHOOSEENUMITEMDIALOG_H
#define CHOOSEENUMITEMDIALOG_H

#include <QDialog>

namespace Ui {
class ChooseEnumItemDialog;
}

class ChooseEnumItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseEnumItemDialog(QWidget *parent = nullptr);
    ~ChooseEnumItemDialog();
    QStringList Signal_Info_Property(QString s);
    QStringList Signal_Info_Property_List;
signals:
    void Send_Property_Choice(QString);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::ChooseEnumItemDialog *ui;
};

#endif // CHOOSEENUMITEMDIALOG_H
