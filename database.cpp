#include "database.h"

Database::Database(QString userName)
{
    this->tabelName = userName;
    createConnection();
    createUserTable();
    createVideoTable();
}

void Database::saveLargePackage(double time, LargePackage pkg, QString type)
{
    QString cmd = QString("INSERT INTO %1 VALUES"
                          "(%2, '%3', %4, %5, %6, %7, %8, %9, %10, %11, %12, %13);")
            .arg(this->tabelName)
            .arg(QString::number(time, 'f', 0))
            .arg(type)
            .arg(pkg.delta)
            .arg(pkg.theta)
            .arg(pkg.lowAlpha)
            .arg(pkg.highAlpha)
            .arg(pkg.lowBeta)
            .arg(pkg.highBeta)
            .arg(pkg.lowGamma)
            .arg(pkg.midGamma)
            .arg(pkg.attention)
            .arg(pkg.meditation);
    exec(cmd);
}

void Database::createUserTable()
{
    QString cmd = QString("CREATE TABLE IF NOT EXISTS %1"
                          "(time text,"
                          "type text,"
                          "delta integer,"
                          "theta integer,"
                          "lowAlpha integer,"
                          "highAlpha integer,"
                          "lowBeta integer,"
                          "highBeta integer,"
                          "lowGamma integer,"
                          "midGamma integer,"
                          "attention integer,"
                          "meditation integer);")
            .arg(this->tabelName);
    exec(cmd);
}
void Database::createVideoTable()
{
    QString cmd = QString("CREATE TABLE IF NOT EXISTS VideoType"
                          "(videoType text UNIQUE);");
    exec(cmd);
//    updateVideoTable();
}
void Database::updateVideoTable()
{
    QDir VideoSrc;
    VideoSrc.setPath(QDir::currentPath() + "/videoSrc");
    VideoSrc.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfo t;
    foreach(t, VideoSrc.entryInfoList())
    {
        QString cmd = QString("INSERT OR IGNORE INTO VideoType VALUES('%1');").arg(t.baseName());
        exec(cmd);
    }
}
bool Database::createConnection()
{
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "user"));
    QString fileName = "EEG.db";
    db->setDatabaseName(fileName);
    if(!db->open())
    {
        QMessageBox::warning(NULL,
                             "ERROR",
                             db->lastError().text());
        return false;
    }
    query = new QSqlQuery(*db);
    return true;
}

bool Database::exec(const QString cmd)
{
//    QSqlQuery query(*db);
    return query->exec(cmd);
}

void Database::close()
{
    databaseBackup();
    db->close();
}

void Database::databaseBackup()
{
    // 备份数据库文件
    // 可以使用sqlite3.exe备份sql文件，或者直接复制db文件
    // 建议直接复制db文件，命名为*.dbbkp
    QProcess s;
    s.start("copy EEG.db EEG.dbbkp");
    s.waitForStarted();

}
