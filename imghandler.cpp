#include "imghandler.h"
#include "QFileDialog"

ImgHandler::ImgHandler(QObject *parent) : QObject(parent)
{
    cbr = new captchBreaker(this);
    connect(cbr,SIGNAL(ConsoleLog(QString)),this,SIGNAL(ConsoleLog(QString)));
    connect(cbr,SIGNAL(solvedNumber(int,int)),this,SLOT(solvedNumber(int,int)));

    voter = new TopVoter(this);
    connect(voter,SIGNAL(ConsoleLog(QString)),this,SIGNAL(ConsoleLog(QString)));
    connect(voter,SIGNAL(GetCaptchaAgain()),this,SLOT(startWork()));

    connect(voter,SIGNAL(alreadyVotedIp()),this,SLOT(goNextProxy()));
    connect(voter,SIGNAL(DeadProxy()),this,SLOT(goNextProxy()));
    connect(voter,SIGNAL(VoteError()),this,SLOT(goNextProxy()));
    connect(voter,SIGNAL(VoteOk()),this,SLOT(goNextProxy()));
    connect(voter,SIGNAL(ForbiddenProxy()),this,SLOT(goNextProxy()));


    //starting new thread
    QThread *t= new QThread();
    t->start();
    while(!t->isRunning()){}
    attempts = 0;lastProxiId=-1;

    //making parser
    Renderer = new Render();
    connect(Renderer,SIGNAL(sendReportCaptcha(int,QWebView*)),this,SLOT(saveCaptcha(int,QWebView*)));

}

void ImgHandler::saveCaptcha(int number, QWebView *wv){
    QList<QNetworkCookie>  list = wv->page()->networkAccessManager()->cookieJar()->cookiesForUrl(QUrl(Renderer->_url.toLocal8Bit()));
    for(int i=0;i<list.size();i++)
        if(list.at(i).name()=="evolist_s")
            voter->setCookie(list.at(i).value());

    wv->resize(wv->page()->mainFrame()->contentsSize());
    QPixmap p = QPixmap::grabWidget(wv);
    p.save("asdasd.png");
    qApp->exit();
    p = p.copy(1,152,635,172);

    solvedCount=0;
    solvedN.clear();
    solvedN.append("00000");
    p.save("lastcaptch.png");
    cbr->solve(&p);

    emit ConsoleLog(tr("Rendered. Start solving..."));
}

void ImgHandler::startWork(){
    attempts++;
    if(attempts>3){
        goNextProxy();
        return;
    }
    emit ConsoleLog(tr("Requesting"));
    Renderer->getPage("http://game100rus.com/");

}

void ImgHandler::loadProxies(){
    QString fileName = QFileDialog::getOpenFileName(0,
         tr("Open proxy list"), QDir::currentPath(), tr("Text files (*.txt)"));
    if(fileName.isEmpty())
        return;
    QFile f(fileName);
    if(!f.open(QIODevice::ReadOnly)){
        emit ConsoleLog(tr("Cannot open file T____T HELP!"));
        return;
    }
    QByteArray arr = f.readAll().replace("\r","");
    QStringList dataStrings = QString(arr).split("\n");
    f.close();
    if(dataStrings.size()==0){
        emit ConsoleLog(tr("No proxy info there >___<"));
        return;
    }

    for(int i=0; i<dataStrings.size();i++){
        QStringList proxyInfo = dataStrings.at(i).split(":");
        if(proxyInfo.size()<2 || proxyInfo.size()>3)
            continue;

        if(proxyListHosts.indexOf(proxyInfo.at(0))!=-1)
            continue;

        QNetworkProxy p;
        p.setHostName(proxyInfo.at(0));

        bool ok;
        p.setPort(proxyInfo.at(1).toInt(&ok));
      /*  if(p.port()!=80 && p.port()!=8080)
            continue;/**/

        if(!ok)
            continue;

        if(proxyInfo.size()==3){
            if(proxyInfo.at(2).toLocal8Bit()=="HTTP" || proxyInfo.at(2).toLocal8Bit()=="HTTPS")
                p.setType(QNetworkProxy::HttpProxy);
            else if(proxyInfo.at(2).toLocal8Bit()=="socks4/5")
                p.setType(QNetworkProxy::Socks5Proxy);
            else continue;
        } else {
            p.setType(QNetworkProxy::HttpProxy);
        }

        proxyListHosts.append(proxyInfo.at(0));
        proxyList.append(p);
    }
    emit ConsoleLog(tr("Current proxy count: "+QString::number(proxyList.size()).toLocal8Bit()));
}

void ImgHandler::solvedNumber(int position, int number){
    solvedCount++;
    QString n = QString::number(number);
    solvedN[position]=n.at(0);

    if( solvedCount == 5 ){
        emit ConsoleLog(tr("solved " + solvedN.toLocal8Bit()));
        voter->addVoice(solvedN);
    }
}

void ImgHandler::setProxy(QNetworkProxy p){
    attempts = 0;
    voter->setProxy(p);
}

void ImgHandler::goNextProxy(){
    lastProxiId++;
    if(proxyList.size()<=lastProxiId){
        emit ConsoleLog(tr("<font color=\"red\">No more proxies</font>"));
        emit ToggleWorkButton();
        return;
    }
    setProxy(proxyList.at(lastProxiId));
    startWork();
}
