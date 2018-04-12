#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QMainWindow>
#include <QtNetwork>
#include <QTimer>

namespace Ui {
class TcpServer;
}

class TcpServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit TcpServer(QWidget *parent = 0);
    ~TcpServer();

    bool checkSignIn(QString name,QString passward);
    bool checkSignUp(QString name,QString passward);
    bool checkUpdate(QString name,QString passward);
protected:
    void init();
signals:
    void userSend();
    void outLine();

private slots:
    void on_startBtn_clicked();

    void acceptConnection();

    void receiveData();

    void displayError(QAbstractSocket::SocketError);

    void sendUserList();

    void removeUser();

    void sendOutlineMessage();
private:
    QString getip();
    Ui::TcpServer *ui;
    QTcpSocket *tcpSocket;
    QTcpServer *tcpServer;
    QTimer *timer;
    QMap<QTcpSocket*,QString> users;
    QMap<QString,QString> mess;
};

#endif // TCPSERVER_H
