#include "topvoter.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

TopVoter::TopVoter(QObject *parent) :
    QObject(parent){
    man=0;
    aborted=false;
    deadProxyTimer.setInterval(120*1000);
    connect(&deadProxyTimer,SIGNAL(timeout()),this,SLOT(ProxyIsDead()));
    srand(time(NULL));

    ratedIps.clear();
    QFile f("rateIps");
    if(f.exists()){
        f.open(QIODevice::ReadOnly);
        ratedIps.append(QString(f.readAll()).split("\n"));
        f.close();
    }
}
void TopVoter::ProxyIsDead(){
    qWarning()<<"deadproxy";
    if(!lastReply || aborted)
        return;
    deadProxyTimer.stop();
    lastReply->abort();
    emit DeadProxy();
    emit ConsoleLog(tr("<font color=\"red\">Dead Proxy</font>"));
    aborted=true;

}

void TopVoter::addVoice(QString num){
    number.clear();
    number.append(num);
    if(this->man)
        this->man->deleteLater();

    man = new QNetworkAccessManager();
    man->setProxy(this->proxy);


    connect(man , SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    connect(man,SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),this,SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));

    QNetworkRequest request;
    request.setRawHeader("Cookie", "evolist_s="+cookie.toLocal8Bit());
    request.setUrl(QUrl("http://game100rus.com/icaptcha.php?do=checkcaptcha&c="+number.toLocal8Bit()));
    aborted=false;
    lastReply = man->get(request);
    deadProxyTimer.start();
    emit ConsoleLog(tr("Checking captcha"));
}

void TopVoter::sslErrors ( QNetworkReply * reply, const QList<QSslError> & errors ){
    emit ConsoleLog(tr("ssl error"));
    for(int i=0;i<errors.size();i++){
        emit ConsoleLog(errors.at(i).errorString());
    }
}

void TopVoter::setCookie(QString c){
    cookie.clear();
    cookie.append(c);
}

void TopVoter::replyFinished(QNetworkReply *rep){
    if(aborted)
        return;
    aborted = false;
    deadProxyTimer.stop();

    QVariant status = rep->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    QByteArray s = rep->readAll();
    if(rep->url().toString().indexOf("icaptcha.php?do=checkcaptcha")!=-1){
        if(status.toInt()==200 && s.indexOf(">OK<")!=-1){//captcha ok
            QNetworkRequest request;
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

            request.setRawHeader("Cookie", "evolist_s="+cookie.toLocal8Bit());
            request.setRawHeader("Host", "game100rus.com");
            request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
            request.setRawHeader("Referer", "http://game100rus.com/index.php?do=votes&id=15839");
            request.setRawHeader("x-insight", "activate");
            request.setRawHeader("Accept-Encoding", "deflate");
            request.setRawHeader("User-Agent", getRandomUserAgent().toLocal8Bit());

            QUrl params;
            params.addQueryItem("vote", "1");
            params.addQueryItem("yes", "Yes");
            params.addQueryItem("captcha", number.toLocal8Bit());
            request.setUrl(QUrl("http://game100rus.com/index.php?do=votes&id=15839"));
            emit ConsoleLog(tr("Captcha <font color=\"green\">ok</font>. Voting..."));
            lastReply = man->post(request,params.encodedQuery());
            deadProxyTimer.start();
        } else if(s.indexOf("ERROR: Forbidden")!=-1
                  || s.indexOf("HTTP Proxy Error")!=-1
                  || s.indexOf("Connection timeout")!=-1){
            emit ConsoleLog(tr("Captch <font color=\"red\">BAD PROXY!</font>"));
            emit ForbiddenProxy();
            lastReply=0;
        } else {
            emit ConsoleLog(tr("Captch <font color=\"red\">wrong</font>."));
            emit GetCaptchaAgain();
            lastReply=0;
        }
    } else if(rep->url().toString().indexOf("index.php?rate")!=-1){
        emit ConsoleLog(tr("Rate finished"));
        emit VoteOk();
        lastReply=0;
    } else {
        if(status.toInt()==200 && s.indexOf("You have already voted before. You can only vote once every rerank of topsite")!=-1){

            QFile f("goodProxy.txt");
            f.open(QIODevice::Append);
            QString s;
            s.append(proxy.hostName());
            s.append(":");
            s.append(QString::number(proxy.port()));
            s.append(":");
            if(proxy.type()==QNetworkProxy::HttpProxy)
                s.append("HTTP");
            else
                s.append("socks4/5");
            s.append("\r\n");
            f.write(s.toLocal8Bit());
            f.close();

            emit ConsoleLog(tr("<font color=\"orange\">Already voted</font> from ip "+man->proxy().hostName().toLocal8Bit()));
            emit alreadyVotedIp();
            lastReply=0;
        }
        else if(status.toInt()==200 && s.indexOf("+ ----- CSS used in default style   ----- +")!=-1 && s.length()>20000){
            emit ConsoleLog(tr("Vote <font color=\"green\">ok</font> from ip " + man->proxy().hostName().toLocal8Bit()));
            
            QFile f("goodProxy.txt");
            f.open(QIODevice::Append);
            QString s;
            s.append(proxy.hostName());
            s.append(":");
            s.append(QString::number(proxy.port()));
            s.append(":");
            if(proxy.type()==QNetworkProxy::HttpProxy)
                s.append("HTTP");
            else
                s.append("socks4/5");
            s.append("\r\n");
            f.write(s.toLocal8Bit());
            f.close();

            if(ratedIps.indexOf(man->proxy().hostName())==-1){
                QNetworkRequest request;
                request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
                request.setRawHeader("Host", "game100rus.com");
                request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
                request.setRawHeader("x-insight", "activate");
                request.setRawHeader("Accept-Encoding", "deflate");
                request.setRawHeader("User-Agent", getRandomUserAgent().toLocal8Bit());

                int rate=10;
                if(rand()%10>8)
                    rate=9;

                QUrl params;
                params.addQueryItem("rate", QString::number(rate).toLocal8Bit());
                params.addQueryItem("id", "15839");
                request.setUrl(QUrl("http://game100rus.com/index.php?rate"));
                emit ConsoleLog(tr("Adding rate "+QString::number(rate).toLocal8Bit()));
                lastReply = man->post(request,params.encodedQuery());
                deadProxyTimer.start();
                ratedIps.append(man->proxy().hostName());

                QFile f("rateIps");
                f.open(QIODevice::WriteOnly);
                f.resize(0);
                f.write(ratedIps.join(tr("\n")).toLocal8Bit());
                f.close();
            } else {
                emit VoteOk();
                lastReply=0;
            }

        } else {
            QFile f("errorPage_"+QDateTime::currentDateTime().toString().toLocal8Bit());
            if(s.size()>0){
                f.open(QIODevice::WriteOnly);
                f.write(s);
                f.close();
            }
            emit ConsoleLog(tr("<font color=\"red\">Vote Error</font> "+f.fileName().toLocal8Bit()));
            emit VoteError();
            lastReply=0;
        }
    }
}

void TopVoter::setProxy(QNetworkProxy p){
    emit ConsoleLog(tr("<font color=\"blue\">Voter new proxy</font> "+p.hostName().toLocal8Bit()+":"+QString::number(p.port()).toLocal8Bit()));
    this->proxy = QNetworkProxy(p);
}

QString TopVoter::getRandomUserAgent(){
    switch(rand()%12){
    case 0:
        return tr("Mozilla/6.0 (Windows NT 6.2; WOW64; rv:16.0.1) Gecko/20121011 Firefox/16.0.1");
        break;
    case 1:
        return tr("Mozilla/5.0 (Windows NT 6.2; WOW64; rv:16.0.1) Gecko/20121011 Firefox/16.0.1");
        break;
    case 2:
        return tr("Mozilla/5.0 (Windows NT 6.2; Win64; x64; rv:16.0.1) Gecko/20121011 Firefox/16.0.1");
        break;
    case 3:
        return tr("Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:18.0) Gecko/20100101 Firefox/18.0");
        break;
    case 4:
        return tr("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.17 (KHTML, like Gecko) Chrome/24.0.1312.60 Safari/537.17");
        break;
    case 5:
        return tr("Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.15 (KHTML, like Gecko) Chrome/24.0.1295.0 Safari/537.15");
        break;
    case 6:
        return tr("Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.14 (KHTML, like Gecko) Chrome/24.0.1292.0 Safari/537.14");
        break;
    case 7:
        return tr("Mozilla/5.0 (compatible; MSIE 10.6; Windows NT 6.1; Trident/5.0; InfoPath.2; SLCC1; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 2.0.50727) 3gpp-gba UNTRUSTED/1.0");
        break;
    case 8:
        return tr("Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0; SLCC2; Media Center PC 6.0; InfoPath.3; MS-RTC LM 8; Zune 4.7)");
        break;
    case 9:
        return tr("Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0; SLCC2; Media Center PC 6.0; InfoPath.3; MS-RTC LM 8; Zune 4.7");
        break;
    case 10:
        return tr("Opera/9.80 (Windows NT 6.0) Presto/2.12.388 Version/12.14");
        break;
    default :
        return tr("Mozilla/5.0 (Windows NT 6.0; rv:2.0) Gecko/20100101 Firefox/4.0 Opera 12.14");
        break;
    }
}
