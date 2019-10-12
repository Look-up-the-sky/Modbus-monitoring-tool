#ifndef PROJECTCREATDIALOG_H
#define PROJECTCREATDIALOG_H

#include <QDialog>

namespace Ui {
class ProjectCreatDialog;
}

class ProjectCreatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectCreatDialog(QWidget *parent = nullptr);
    ~ProjectCreatDialog();

private:
    Ui::ProjectCreatDialog *ui;
};

#endif // PROJECTCREATDIALOG_H
