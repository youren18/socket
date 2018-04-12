#ifndef P2PDIALOG_H
#define P2PDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTcpServer>
#include <QFile>
#include <QFileDialog>
#include <QTimer>


namespace Ui {
class P2pDialog;
}

class P2pDialog : public QDialog
{
    Q_OBJECT

public:
    explicit P2pDialog(QString &mName, QString &pName, QTcpSocket *ppSocket, QWidget *parent = 0);
    ~P2pDialog();


private slots:
    void on_messageButton_clicked();

    void on_fileButton_clicked();

    void closeEvent(QCloseEvent *);

    void readMessage();

    void readFile();
    void readFile2();

    void sendData();

    void startSend();

    void fs(const QString& a);

    void sendtimeout();
private:
    Ui::P2pDialog *ui;
    QString myName;
    QString peerName;
    QTcpSocket *pSocket;
    QUdpSocket *uSocket;
    QUdpSocket *uRecSocket;
    QString fileName;
    QFile file,recFile;
    QString ip;
    QFileDialog *fDialog;
    bool sendFlag;
    quint64 i;
    quint64 fileByte;
    quint64 recFileByte;
    QString recFileName;
    QByteArray ACK;
    QString recFilePath;
    QByteArray line;
    QTimer *timer;
};

#endif // P2PDIALOG_H
