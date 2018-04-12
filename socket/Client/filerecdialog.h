#ifndef FILERECDIALOG_H
#define FILERECDIALOG_H

#include <QDialog>

namespace Ui {
class FileRecDialog;
}

class FileRecDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileRecDialog(QWidget *parent = 0);
    ~FileRecDialog();

private:
    Ui::FileRecDialog *ui;
};

#endif // FILERECDIALOG_H
