#include "chatwindow.h"
#include "ui_chatwindow.h"
#include <QMessageBox>
#include "p2pdialog.h"
#include <QHostInfo>

#define port 8000

ChatWindow::ChatWindow(QString *na, QTcpSocket *tS, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChatWindow),
    tSocket(tS),
    mName(na)
{
    readFlags=false;
    p2pServer = new QTcpServer(this);
    p2pSocket = new QTcpSocket(this);
    ui->setupUi(this);
    ui->userEdit->setText(*mName);
    ui->ipEdit->setText(getip());
    if(!p2pServer->listen(QHostAddress::Any,9001))
    {
        qDebug()<<p2pServer->errorString();
        close();
        return;
    }
    connect(tSocket,&QTcpSocket::readyRead,this,&ChatWindow::readMessage);
    connect(p2pServer,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
    QByteArray by;
    by.append("f#");
    by.append(*mName);
    tSocket->write(by);
}

ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::readMessage()
{
    QTcpSocket *tempTcp=static_cast<QTcpSocket*>(sender());
    QByteArray temp=tempTcp->readAll();
    QString data=QString::fromLocal8Bit(temp);
    QStringList list=data.split("#");
    qDebug()<<temp;
    if(list[0] == "c"){//收到在线用户名列表
        ui->listWidget->clear();
        users.clear();
        for(int i = 1;i < list.size();i++){
            if(list[i]!=*mName){
                ui->listWidget->addItem(list[i]);
                users.append(list[i]);
                qDebug() <<"在线用户"<< list[i];
            }
        }
    }
    else if(list[0] == "p"){//私聊
//        QMap<QString,P2pDialog*>::iterator it = pList.begin();
//        while(it!=pList.end()){
//            if(it.key() == list[2])
//               return;
//            it++;
//        }
        if(list[1]=="false"){
            QMessageBox::information(this,"警告","发送离线消息",QMessageBox::Ok);
            P2pDialog *p2plog = new P2pDialog(*mName,list[2],tSocket,this);
            pList.insert(list[2],p2plog);
            p2plog->show();
            return;
        }
        p2pSocket->connectToHost(list[3],9000);
        P2pDialog *p2plog = new P2pDialog(*mName,list[2],p2pSocket,this);
        pList.insert(list[2],p2plog);
        p2plog->show();
        QString message = "p2p#"+*mName;
        p2pSocket->write(message.toLatin1());
    }
    else if(list[0]=="e"){
        ui->textBrowser->append(list[1]+list[3]);
        ui->textBrowser->append(list[4]);
    }
        return;
}

void ChatWindow::readP2p()
{
    if(readFlags)
        return;
    QTcpSocket *tempTcp=static_cast<QTcpSocket*>(sender());
    QByteArray temp=tempTcp->readAll();
    QString data = QString::fromLocal8Bit(temp);
    qDebug()<<"chat read p2p"<<data;
    QStringList list=data.split("#");
    if(list[0]=="p2p"){
//        QMap<QString,P2pDialog*>::iterator it = pList.begin();
//        while(it!=pList.end()){
//            if(it.key() == list[1])
//                return;
//            it++;
//        }
        P2pDialog *p2plog = new P2pDialog(*mName,list[1],tempTcp,this);
        pList.insert(list[1],p2plog);
        p2plog->show();
        readFlags=true;
    }
}

void ChatWindow::acceptConnection()
{
    p2pSocket = p2pServer->nextPendingConnection();
    readFlags = false;
    connect(p2pSocket,&QTcpSocket::readyRead,this,&ChatWindow::readP2p);
}

QString ChatWindow::getip()
{
    QString localhost = QHostInfo::localHostName();
    QHostInfo hostinfo = QHostInfo::fromName(localhost);
    QList<QHostAddress> al = hostinfo.addresses();
    foreach(QHostAddress ad, al)
        if(ad.protocol() == QAbstractSocket::IPv4Protocol)
            return ad.toString();
    return QString("255.255.255.255");
}


void ChatWindow::on_flushButton_clicked()
{
    QString message="c#getUserList";
    tSocket->write(message.toLatin1());
}

void ChatWindow::on_listWidget_doubleClicked(const QModelIndex &index)
{
    QString str=users.at(index.row());  //获得用户名
    qDebug()<<"双击获取名字"<<str;
    QString message = "p#"+*mName+"#"+str;
    tSocket->write(message.toLatin1());
}


void ChatWindow::on_p2pButton_clicked()
{
    QString name = ui->nameEdit->text();
    QString message = "p#"+*mName+"#"+name;
    tSocket->write(message.toLatin1());
}
