#include "tcpclient.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//情况2
    QTextCodec::setCodecForLocale(codec);

    QApplication a(argc, argv);
    TcpClient w;
    w.setWindowTitle("Client");
    w.show();

    return a.exec();
}
