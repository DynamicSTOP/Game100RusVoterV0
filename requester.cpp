#include "requester.h"

requester::requester(QObject *parent) :
    QObject(parent)
{
    netManager=0;
}

void requester::setNumber(int n){
    number=n;
}
void requester::createManager(){
    netManager = new QNetworkAccessManager();
    data.clear();
    connect(netManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(recivedData(QNetworkReply*)));
}

void requester::getPageData(QUrl url){
    QNetworkRequest request;
    request.setUrl(url);
    netManager->get(request);
}
void requester::recivedData(QNetworkReply* reply){
    data.append(reply->readAll());
    emit jobDone(data);
}
