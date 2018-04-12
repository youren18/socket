#include "regdialog.h"
#include "ui_regdialog.h"
#include <QMessageBox>

RegDialog::RegDialog(QTcpSocket *t,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegDialog),
    tSocket(t)
{
    ui->setupUi(this);
}

RegDialog::~RegDialog()
{
    delete ui;
}

void RegDialog::on_pushButton_clicked()
{
    QString user,psw;
    user = ui->userEdit->text();
    psw = ui->pswEdit->text();
    if(user.isEmpty()||psw.isEmpty()){
        QMessageBox::information(this,"警告","输入不能为空",QMessageBox::Ok);
        return;
    }

    QString mes = "d#"+user+"#"+psw;
    tSocket->write(mes.toLatin1());
    accept();
}
