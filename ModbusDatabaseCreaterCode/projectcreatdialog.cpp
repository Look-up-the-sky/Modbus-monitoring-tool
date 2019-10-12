#include "projectcreatdialog.h"
#include "ui_projectcreatdialog.h"

ProjectCreatDialog::ProjectCreatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProjectCreatDialog)
{
    ui->setupUi(this);
}

ProjectCreatDialog::~ProjectCreatDialog()
{
    delete ui;
}
