#ifndef DATEBASE_H
#define DATEBASE_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include "identifiers.h"
class Database
{
public:
    Database(QString);
    QString tabelName;
    bool createConnection();
    void createUserTable();
    void createVideoTable();
    void saveLargePackage(double, LargePackage, QString);
    bool exec(const QString);
    void close();
    QSqlQuery *query = nullptr;
    QSqlDatabase *db;
    void databaseBackup();
    void updateVideoTable();
};

#endif // DATEBASE_H
