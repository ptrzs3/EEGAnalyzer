#include "manager.h"

Manager::Manager()
{
    dynplot = new DynPlot();
    tgam = new TGAMParser();
    if(!tgam->connectEEGDevice())
    {
        QMessageBox::warning(dynplot->widget,
                             "EEG Analyzer",
                             "No TGAM adapter found");
    }
    videoPlayer = new VideoPlayer();
    connect(videoPlayer->player, &QMediaPlayer::positionChanged, this, &Manager::videoFinish);
    connect(tgam, &TGAMParser::smallPackageReady, dynplot, &DynPlot::updateRawWave);
    connect(tgam, &TGAMParser::largePackageReady, dynplot, &DynPlot::updateWave);
    connect(dynplot->loadVideo, &QPushButton::clicked, this, &Manager::useSuggestion);
    dynplot->loadVideo->setEnabled(!this->username.isEmpty());
    connect(dynplot, &DynPlot::replotFinish, this, &Manager::saveEEGData);
    connect(dynplot->analyze, &QPushButton::clicked, this, &Manager::analyzeInterset);
    connect(dynplot->faceRecognize, &QPushButton::clicked, this, &Manager::faceRecognize);
    connect(dynplot->quit, &QPushButton::clicked, this, [=]{
        playing = false;
        if(eegDatabase != nullptr)
            eegDatabase->close();
        dynplot->close();
        tgam->port->close();
        videoPlayer->close();
        exit(0);
    });
    videoSrc = QDir::currentPath() + "/videoSrc";
    fileFilter<<"*.mp4"<<"*.mkv";
}

void Manager::playVideo()
{
    if(!videoSrc.exists()) {
        QMessageBox::warning(dynplot->widget,
                             "EEG Analyzer",
                             "No video folder found");
        return;
    }
    QDir temp;
    temp.setPath(videoSrc.path());
    temp.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfoList videoTypeList = removeEmptyFolder(temp.entryInfoList());
    if(videoTypeList.size() == 0)
    {
        QMessageBox::warning(dynplot->widget,
                             "EEG Analyzer",
                             "No video file found");
        return;
    }
    int x = QRandomGenerator::global()->bounded(videoTypeList.size());
    temp.setPath(videoTypeList[x].absoluteFilePath());
    type = videoTypeList[x].baseName();
    temp.setFilter(QDir::Files);
    temp.setNameFilters(fileFilter);
    videoTypeList = temp.entryInfoList();
    x = QRandomGenerator::global()->bounded(videoTypeList.size());
    playing = videoPlayer->play(videoTypeList[x].filePath());
    dynplot->loadVideo->setEnabled(!playing);
    dynplot->analyze->setEnabled(!playing);
}

void Manager::videoFinish()
{
    if(playing==true && videoPlayer->player->duration() >0
            && videoPlayer->player->position() == videoPlayer->player->duration()){
        videoPlayer->hide();
        playing = false;
        dynplot->loadVideo->setEnabled(!playing);
        dynplot->analyze->setEnabled(!playing);
        QMessageBox:: StandardButton result = QMessageBox::information(dynplot->widget,
                                 "EEG Analyzer",
                                 "continue or not",
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if(result == QMessageBox::Yes)
            playVideo();
    }
}
void Manager::saveEEGData(double time, LargePackage largePackage)
{
    if(playing) {
        eegDatabase->saveLargePackage(time, largePackage, type);
    }
}

void Manager::useSuggestion()
{
    if(!tgam->connected){
        QMessageBox::warning(dynplot->widget,
                             "EEG Analyzer",
                             "No TGAM adapter found, please plug it in"
                             " and restart the software");
        dynplot->loadVideo->setEnabled(false);
        return;
    }
    if(QMessageBox::information(dynplot->widget,
                             "EEG Analyzer",
                             "Please wear the brain wave acquisition equipment"
                                " with the help of the staff",
                             "Start now", "Later") == 0)
        playVideo();
}

QFileInfoList Manager::removeEmptyFolder(QFileInfoList src)
{
    QFileInfo t;
    QFileInfoList dst;
    foreach(t, src)
    {
        if(!QDir(t.absoluteFilePath()).isEmpty())
        {
            dst.append(t);
        }
    }
    return dst;
}

QString Manager::executeScript(TCHAR cmd[])
{
    //创建匿名管道
    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
    HANDLE hRead, hWrite;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0))
    {
        return QString::number(-2);
    }

    //设置命令行进程启动信息(以隐藏方式启动命令并定位其输出到hWrite)
    STARTUPINFO si = {sizeof(STARTUPINFO)};
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;

    //启动命令行
    PROCESS_INFORMATION pi;
    if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
    {
        return QString::number(-3);
    }

    //立即关闭hWrite
    CloseHandle(hWrite);

    //读取命令行返回值
    std::string strRet;
    char buff[1024] = {0};
    DWORD dwRead = 0;
    while (ReadFile(hRead, buff, 1024, &dwRead, NULL))
    {
        strRet.append(buff, dwRead);
    }
    CloseHandle(hRead);
    return QString::fromStdString(strRet.c_str()).simplified();
}

bool Manager::faceRecognize()
{
    if(QCameraInfo::availableCameras().length() == 0){
        QMessageBox::warning(dynplot->widget,
                             "EEG Analyzer",
                             "No camera detected");
        return false;
    }
    QString s = QDir::currentPath() + "/Scripts/FaceRecognition.exe";
    QFile rec(s);
    if(!rec.exists())
    {
        QMessageBox::warning(dynplot->widget,
                                     "EEG Analyzer",
                                     "./Scripts/FaceRecognition.exe missing");
                dynplot->faceRecognize->setEnabled(false);
        return false;
    }
    s.append(" aE1pG0aB1gE4");
    wchar_t* dst = QString2wchat_t(s);
    QString ret = executeScript(dst);
    if(ret == "-1"){
        QMessageBox::warning(dynplot->widget,
                             "EEG Analyzer",
                             "Face detector file(s) missing");
        dynplot->faceRecognize->setEnabled(false);
        return false;
    } else if(ret == "-2") {
        if(QMessageBox::information(dynplot->widget,
                             "EEG Analyzer",
                             "No model found, try to input face?",
                                    "Yes", "No") == 0){
            return faceRegistration();
        } else {
            return false;
        }
    } else if(ret == "-3") {
        if(QMessageBox::information(dynplot->widget,
                                     "EEG Analyzer",
                                     "unrecognized face",
                                    "Input face", "Leave") == 0){
            return faceRegistration();
        }
    } else {
        this->username = ret;
        dynplot->loadVideo->setEnabled(!this->username.isEmpty());
        dynplot->faceRecognize->setEnabled(this->username.isEmpty());
        // 通过人脸识别后才将数据库实例化，并传过去用户名，比如：User_cgj
        // 将以用户名创建数据库表
        if(eegDatabase == nullptr) {
            eegDatabase = new Database(this->username);
        }
        getVideoTypes();
        dynplot->updateTimeOnce();
        dynplot->canDraw = true;
        return true;
    }
}
wchar_t* Manager::QString2wchat_t(QString src)
{
    std::string temp = src.toStdString();
    int len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)temp.c_str(), -1, NULL, 0);
    wchar_t *dst = new wchar_t[len + 1];
    memset(dst, 0, len * 2 + 2);
    MultiByteToWideChar(CP_ACP , 0 , (LPCSTR)temp.c_str() , -1 , (LPWSTR)dst , len);
    return dst;
}
bool Manager::faceRegistration()
{
    QString s = QDir::currentPath() + "/Scripts/FaceRegistration.exe";
    QFileInfo reg(s);
    if(!reg.exists())
    {
        QMessageBox::warning(dynplot->widget,
                                     "EEG Analyzer",
                                     "./Scripts/FaceRegistration.py missing");
                dynplot->faceRecognize->setEnabled(false);
        return false;
    }
    bool isOK;
    QString input = QInputDialog::getText(dynplot->widget,
                                         "EEG Analyzer",
                                         "who are you?",
                                         QLineEdit::Normal,
                                         "Only accept Chinese characters, letters, numbers",
                                         &isOK);
    if(isOK){
        input = usernameCheck(input);
        QString cmd = QString("%1 %2")
                .arg(s)
                .arg(input);
        wchar_t* dst = QString2wchat_t(cmd);
        QString ret = executeScript(dst);
        // 缺少探测器文件
        if(ret == "-1") {
            QMessageBox::warning(dynplot->widget,
                                 "EEG Analyzer",
                                 "Face detector file(s) missing");
            dynplot->faceRecognize->setEnabled(false);
            return false;
        } else if(ret == "0"){
            QString w = QString("Face collection succeeded: %1").arg(input);
            QMessageBox::warning(dynplot->widget,
                                 "EEG Analyzer",
                                 w);
            return true;
        }
    } else {
        // 不录入人脸
        return false;
    }
}

QString Manager::usernameCheck(QString src)
{
    QString prefix = "User_";
    QRegExp re;
    re.setPattern("[^\u4e00-\u9fa5a-zA-Z0-9]");
    QString dst = src.replace(re, "");
    return prefix + dst;
}

bool Manager::getTotalNumberOfRecords()
{
    QString cmd = QString("select count(*) from %1;").arg(username);
    QSqlQuery query(*eegDatabase->db);
    query.exec(cmd);
    query.next();
    int count = query.value(0).toInt();
    if(count < 6*60*60){
        if(QMessageBox::information(dynplot->widget,
                                 "EEG Analyzer",
                                 "Insufficient amount of data, whether to continue the analysis",
                                    "Yes", "No")==0){
            return true;
        } else {
            return false;
        }
    } else {
        return true;
    }
}
void Manager::getVideoTypes()
{
    QString cmd =  "select videoType from VideoType;";
    QSqlQuery query(*eegDatabase->db);
    if(query.exec(cmd)){
        while(query.next()){
            videoTypeList<<query.value(0).toString();
        }
    } else {
        QMessageBox::warning(dynplot->widget,
                             "EEG Analyzer",
                             "Failed to execute database query");
        return;
    }
}
void Manager::analyzeResultInitialization()
{
    foreach(QString t, videoTypeList) {
        analyzeResult.insert(t, 0);
    }
}
void Manager::analyzeInterset()
{
    QString name_t;
    if(username.isEmpty()){
        bool isOK;
        QString t = QInputDialog::getText(dynplot->widget,
                                          "EEG Analyzer",
                                          "Input username",
                                          QLineEdit::Normal,
                                          "example: cgj",
                                          &isOK);
        if(isOK){
            name_t = "User_" + t;
            eegDatabase = new Database(name_t);
            getVideoTypes();
//            name_t = t;
        } else {
            return;
        }
    } else {
        name_t = username;
    }
    QSqlQuery query(*eegDatabase->db);
    QString cmd;
    cmd = QString("select count(*) from sqlite_master where type='table' and name='%1';")
            .arg(name_t);
    query.clear();
    query.exec(cmd);
    query.next();
    if(query.value(0).toInt() ==0){
        QMessageBox::warning(dynplot->widget,
                             "EEG Analyzer",
                             "No record for this user, please check your input");
        return;
    }
    if(!getTotalNumberOfRecords())
        return;
    if(videoTypeList.isEmpty())
        return;
    analyzeResultInitialization();
    foreach(QString t, videoTypeList)
    {
        cmd = QString("select count(*) from %1 where type='%2';")
                .arg(name_t)
                .arg(t);
        query.clear();
        query.exec(cmd);
        query.next();
        int a = query.value(0).toInt();
        if(a == 0)
            continue;
        cmd = QString("select attention from %1 where type='%2';")
                .arg(name_t)
                .arg(t);
        query.clear();
        query.exec(cmd);
        double sum = 0;
        int cnt = 0;
        while(query.next()){
            int v = query.value(0).toInt();
            sum += v;
            cnt = query.at() + 1;
        }
        analyzeResult.insert(t,sum/cnt);
    }
    StaPlot *staplot = new StaPlot(name_t.replace("User_", ""), analyzeResult);
    staplot->show();
}
