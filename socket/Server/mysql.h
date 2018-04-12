#ifndef MYSQL_H
#define MYSQL_H

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

class MySql
{
public:
    MySql();
    void initsql();
    void createtable();
    bool loguser(QString name,QString passward);
    bool signup(QString name,QString passward);
    bool pswupdate(QString name,QString passward);

private:
    QSqlQuery *query;
};

#endif // MYSQL_H
