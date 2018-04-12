#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "mysql.h"
#include <QCryptographicHash>

TcpServer::TcpServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TcpServer)
{
    ui->setupUi(this);

    init();
}

TcpServer::~TcpServer()
{
    delete ui;
}

QString TcpServer::getip()
{
    QString localhost = QHostInfo::localHostName();
    QHostInfo hostinfo = QHostInfo::fromName(localhost);
    QList<QHostAddress> al = hostinfo.addresses();
    foreach(QHostAddress ad, al)
        if(ad.protocol() == QAbstractSocket::IPv4Protocol)
            return ad.toString();
    return QString("255.255.255.255");
}

void TcpServer::init()
{
    this->tcpServer=new QTcpServer(this);
    this->tcpSocket=new QTcpSocket(this);
    ui->textEdit->setText(getip());

    connect(this,&TcpServer::userSend,this,&TcpServer::sendUserList);
    connect(this,&TcpServer::outLine,this,&TcpServer::sendOutlineMessage);
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),
                     this,SLOT(displayError(QAbstractSocket::SocketError)));
}


void TcpServer::removeUser()
{
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());
    qDebug()<< "下线";
    QMap<QTcpSocket*,QString>::iterator it=users.begin();
    QString message;
    while(it!=users.end())
    {
        if(it.key()==socket){
            message = it.value();
            break;
        }
        ++it;
    }
    message+="下线";
    ui->textBrowser->append(message);
    users.remove(socket);
    emit userSend();
}

void TcpServer::sendUserList()
{
    QString message="c";
    QMap<QTcpSocket*,QString>::iterator it=users.begin();
    while(it!=users.end())
    {
        message+="#";
        message+=it.value();
        ++it;
    }
    if(message=="c")
        return;
    it=users.begin();
    while(it!=users.end())
    {
        it.key()->write(message.toLatin1());
        qDebug()<<message;
        ++it;
    }

}

void TcpServer::sendOutlineMessage()
{

}

void TcpServer::receiveData()
{
    QTcpSocket *tempTcp=static_cast<QTcpSocket*>(sender());
    QString data=tempTcp->readAll();
    QStringList list=data.split("#");
    qDebug()<<data;
    bool ret=0;
    if(list[0]=="a")     //注册
        ret=checkSignUp(list[1],list[2]);
    else if(list[0]=="b"){  //登录
        ret=checkSignIn(list[1],list[2]);
        if(ret){
            users.insert(tempTcp,list[1]);
            ui->textBrowser->append(list[1] + "上线");
            connect(tempTcp,SIGNAL(disconnected()),this,SLOT(removeUser()));

        }
    }
    else if(list[0]=="c"){//获取在线列表
        qDebug() << "获取好友";
        sendUserList();
        return;
    }
    else if(list[0] == "d"){//找回密码
        qDebug()<<"找回密码";
        ret = checkUpdate(list[1],list[2]);

    }
    else if(list[0]=="e"){
        qDebug()<<"离线消息";
        mess.insert(list[2],data);
        return;
    }
    else if(list[0]=="f"){
        QMap<QString,QString>::iterator its = mess.begin();
        while(its!=mess.end()){
            if(its.key()==list[1]){
                tcpSocket->write(its.value().toLatin1());
                mess.remove(list[1]);
                qDebug()<<its.value();
            }
            its++;
        }
    }
    else if(list[0]=="p"){//申请私聊
        QMap<QTcpSocket*,QString>::iterator it=users.begin();
        QString ip="";
        while(it!=users.end())
        {
            if(it.value()==list[2]){
                ip = it.key()->peerAddress().toString();
                ip.remove(QString("::ffff:"),Qt::CaseSensitive);
                ret = true;
                qDebug()<<"获取ip"<<ip;

            }
            it++;
        }
        if(ip.isEmpty()){
            ret = false;
            ip = getip();
        }
        QString message = "p#";

        if(ret){
            message += "true#";
        }
        else
            message += "false#";
        message += list[2]+"#"+ip;
        tcpSocket->write(message.toLatin1());
        qDebug()<<"私聊返回"<<message;
        return;
    }
    else
        return;
    QString sendData=list[0];
    if(ret)
        sendData+="#true";
    else
        sendData+="#false";
    tcpSocket->write(sendData.toLatin1());
}

void TcpServer::on_startBtn_clicked()
{
    ui->startBtn->setEnabled(false);
    if(!tcpServer->listen(QHostAddress::Any,8000))
    {
        qDebug()<<tcpServer->errorString();
        close();
        return;
    }
}


void TcpServer::acceptConnection()
{
    tcpSocket=tcpServer->nextPendingConnection();
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(receiveData()));
    QString str("l#true");
    tcpSocket->write(str.toLatin1());
}

void TcpServer::displayError(QAbstractSocket::SocketError)
{
    qDebug()<<tcpSocket->errorString();
    tcpSocket->close();
}

bool TcpServer::checkSignIn(QString name,QString passward)
{
    MySql *mysql=new MySql();

    QString usernamemd5;
    QString pwdmd5;
    QByteArray bb;
    bb = QCryptographicHash::hash (passward.toLatin1(), QCryptographicHash::Md5 );
    pwdmd5.append(bb.toHex());
    bb = QCryptographicHash::hash(name.toLatin1(),QCryptographicHash::Md5);
    usernamemd5.append(bb.toHex());

    bool ret=mysql->loguser(usernamemd5,pwdmd5);
    return ret;
}

bool TcpServer::checkSignUp(QString name, QString passward)
{
    MySql *mysql=new MySql();

    QString usernamemd5;
    QString pwdmd5;
    QByteArray bb;
    bb = QCryptographicHash::hash (passward.toLatin1(), QCryptographicHash::Md5 );
    pwdmd5.append(bb.toHex());
    bb = QCryptographicHash::hash(name.toLatin1(),QCryptographicHash::Md5);
    usernamemd5.append(bb.toHex());

    bool ret=mysql->signup(usernamemd5,pwdmd5);
    return ret;
}
bool TcpServer::checkUpdate(QString name, QString passward)
{
    MySql *mysql=new MySql();

    QString usernamemd5;
    QString pwdmd5;
    QByteArray bb;
    bb = QCryptographicHash::hash (passward.toLatin1(), QCryptographicHash::Md5 );
    pwdmd5.append(bb.toHex());
    bb = QCryptographicHash::hash(name.toLatin1(),QCryptographicHash::Md5);
    usernamemd5.append(bb.toHex());

    bool ret=mysql->pswupdate(usernamemd5,pwdmd5);
    return ret;
}
