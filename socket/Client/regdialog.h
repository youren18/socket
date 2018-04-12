#ifndef REGDIALOG_H
#define REGDIALOG_H

#include <QDialog>
#include <QTcpSocket>

namespace Ui {
class RegDialog;
}

class RegDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegDialog(QTcpSocket *t=0,QWidget *parent = 0);
    ~RegDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::RegDialog *ui;
    QTcpSocket *tSocket;

};

#endif // REGDIALOG_H
