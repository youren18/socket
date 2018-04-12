#ifndef CHATWINDOW_H
#define CHATWINDOW_H
#include <QTcpSocket>
#include <QMainWindow>
#include <QTcpServer>
#include "p2pdialog.h"

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChatWindow(QString *na = 0, QTcpSocket *tS = 0, QWidget *parent = 0);
    ~ChatWindow();

private slots:

    void on_flushButton_clicked();

    void readMessage();

    void readP2p();

    void on_listWidget_doubleClicked(const QModelIndex &index);

    void acceptConnection();
    void on_p2pButton_clicked();

private:
    QString getip();
    Ui::ChatWindow *ui;
    QTcpSocket *tSocket;
    QTcpSocket *p2pSocket;
    QTcpServer *p2pServer;
    QString *mName;
    QStringList users;
    QMap<QString,P2pDialog*> pList;
    bool readFlags;
};

#endif // CHATWINDOW_H
