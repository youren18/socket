#include "p2pdialog.h"
#include "ui_p2pdialog.h"
#include <QDateTime>
#include <QMessageBox>
#include "filerecdialog.h"
P2pDialog::P2pDialog(QString &mName, QString &pName, QTcpSocket *ppSocket, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::P2pDialog),
    myName(mName),
    peerName(pName),
    pSocket(ppSocket)
{
    ACK.append("1");
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&P2pDialog::sendtimeout);
    fDialog = new QFileDialog(this);
    fDialog->setFileMode(QFileDialog::Directory);
    fDialog->hide();

    sendFlag = false;
    ui->setupUi(this);
    ui->nameEdit->setText(peerName);

    connect(pSocket,&QTcpSocket::readyRead,this,&P2pDialog::readMessage);
    connect(fDialog,&QFileDialog::fileSelected,this,&P2pDialog::fs);
}

P2pDialog::~P2pDialog()
{
    delete ui;
}

void P2pDialog::on_messageButton_clicked()
{
    QString str = ui->textEdit->toPlainText();
    if(str.isEmpty())
        return;
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString message = "e#"+myName+"#"+peerName+"#"+time+"#"+str;
    qDebug()<<"p2p send"<<message;
    QByteArray mes;
    mes.append(message);
    pSocket->write(mes);
    ui->textEdit->clear();
    ui->textBrowser->append("Me("+myName+")"+time);
    ui->textBrowser->append(str);
    connect(pSocket,&QTcpSocket::readyRead,this,&P2pDialog::readMessage);

}

void P2pDialog::readMessage()
{
    QTcpSocket *tempTcp=static_cast<QTcpSocket*>(sender());
    QByteArray temp = tempTcp->readAll();
    QString data=QString::fromLocal8Bit(temp.data());
    qDebug()<<"p2p read p2p"<<data;
    QStringList list=data.split("#");
    if(list[0]=="e"){
    ui->textBrowser->append(list[1]+list[3]);
    ui->textBrowser->append(list[4]);
    }
    else if(list[0]=="file"){
        ip = pSocket->peerAddress().toString();
        ip.remove(QString("::ffff:"),Qt::CaseSensitive);
        qDebug()<<ip;

        uRecSocket = new QUdpSocket(this);
        uRecSocket->bind(QHostAddress::Any,8501);//接受
        connect(uRecSocket,&QUdpSocket::readyRead,this,&P2pDialog::readFile);

        recFileName = list[1];
        recFileByte = list[2].toULongLong();
        qDebug()<<"接收文件"<<recFileName<<recFileByte;
        sendFlag = false;
        QMessageBox::warning(this,tr("通知"),tr("收到文件！"),QMessageBox::Yes);
        fDialog->show();

    }
    else if(list[0]=="over"){
        if(timer->isActive()){
            timer->stop();
        }
        file.close();
        QMessageBox::warning(this,tr("通知"),tr("接收完成！"),QMessageBox::Yes);

    }
    else
        return;
}

void P2pDialog::sendtimeout()
{
    qDebug()<<"重发";
    uSocket->writeDatagram(line,QHostAddress(ip),8501);
    qDebug()<< "send over!"<< i << line.size();
    if(line.size() <8000){
        QByteArray yes;
        yes.append("over#文件接收完成！");
        pSocket->write(yes);
        QMessageBox::warning(this,tr("通知"),tr("发送成功！"),QMessageBox::Yes);
    }
}

void P2pDialog::on_fileButton_clicked()
{
    ip = pSocket->peerAddress().toString();
    ip.remove(QString("::ffff:"),Qt::CaseSensitive);
    qDebug()<<ip;

    fileName = QFileDialog::getOpenFileName(this,tr("选择文件"));
    if(fileName.isEmpty()){
        QMessageBox::warning(this, "File Not Open",
            "选择文件错误!请重试...", QMessageBox::Ok);
        return;
    }
    ui->textBrowser->append(tr("发送文件..."));
    startSend();
}

void P2pDialog::sendData()
{
    qDebug()<<"fasong";
    if (!file.atEnd()) {
        line.clear();
        line = file.read(8000);
        uSocket->writeDatagram(line,QHostAddress(ip),8501);
        i++;
        qDebug()<< "send over!"<< i << line.size();
        if(line.size() <8000){
            QByteArray yes;
            yes.append("over#文件接收完成！");
            pSocket->write(yes);
            QMessageBox::warning(this,tr("通知"),tr("发送成功！"),QMessageBox::Yes);
        }
    }

}

void P2pDialog::startSend()
{
    i = 0;
    qDebug()<<"开始发送文件";
    file.setFileName(fileName);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"打开文件","无法打开文件!!!",QMessageBox::Ok);
        return;
    }
    sendFlag = true;
    QString fName = fileName.right(fileName.size() -
                    fileName.lastIndexOf('/') - 1);
    fileByte = file.size();
    QString str = "file#"+fName+"#";
    qDebug()<<"文件名"<<str;
    str+=QString::number(fileByte,10);
    QByteArray by;
    by.append(str);
    qDebug()<<QString::fromLocal8Bit(by);
    pSocket->write(by);

    uSocket = new QUdpSocket(this);
    uSocket->bind(QHostAddress::Any,8500);//发送
    connect(uSocket,&QUdpSocket::readyRead,this,&P2pDialog::readFile2);
}

void P2pDialog::readFile2()
{
    while (uRecSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(uRecSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        uSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        qDebug() << datagram;
        if(datagram == "1"){
            qDebug() << "send OK!";
            timer->start(500);
            sendData();
        }
    }
}

void P2pDialog::readFile()
{
        while (uRecSocket->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(uRecSocket->pendingDatagramSize());
            QHostAddress sender;
            quint16 senderPort;
            uRecSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

            file.write(datagram.data(),datagram.size());
            i++;
            uRecSocket->writeDatagram("1", sender ,8500);
            qDebug() << i << datagram.size();
        }

}

void P2pDialog::fs(const QString &a)
{
    recFilePath.append(a);
    recFilePath.append("/");
    recFilePath.append(recFileName);
    qDebug()<<"接受路径";
    qDebug()<<a;
    qDebug()<<recFilePath;

    recFile.setFileName(recFilePath);
    if(!recFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Unbuffered)){
        qDebug()<<"fileopenerror";
        return;
    }
    uSocket->writeDatagram("1",QHostAddress(ip),8501);
    qDebug()<<"fileack";
}

void P2pDialog::closeEvent(QCloseEvent *)
{

}
