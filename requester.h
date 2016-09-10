#ifndef REQUESTER_H
#define REQUESTER_H

#include <QObject>
#include <QtNetwork>

class requester : public QObject
{
    Q_OBJECT
public:
    explicit requester(QObject *parent = 0);
    void setNumber(int n);
    void createManager();
    void getPageData(QUrl url);

private:
    int number;
    QNetworkAccessManager *netManager;
    QByteArray data;

private slots:
    void recivedData(QNetworkReply*);
public slots:


signals:
    void jobDone(QByteArray arr);
};

#endif // REQUESTER_H
