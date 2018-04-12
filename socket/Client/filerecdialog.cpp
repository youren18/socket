#include "filerecdialog.h"
#include "ui_filerecdialog.h"

FileRecDialog::FileRecDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileRecDialog)
{
    ui->setupUi(this);
}

FileRecDialog::~FileRecDialog()
{
    delete ui;
}
