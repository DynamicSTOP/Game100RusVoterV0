#ifndef TOPVOTER_H
#define TOPVOTER_H

#include <QObject>
#include <QtCore>
#include <QtNetwork>

class TopVoter : public QObject
{
    Q_OBJECT
public:
    explicit TopVoter(QObject *parent = 0);
    void setCookie(QString c);

private:
    QNetworkAccessManager *man;
    QString cookie;
    QString number;
    QNetworkProxy proxy;
    QTimer deadProxyTimer;

    QNetworkReply *lastReply;
    bool aborted;
    QString getRandomUserAgent();

    QStringList ratedIps;

signals:
    void alreadyVotedIp();
    void VoteOk();
    void VoteError();
    void DeadProxy();
    void GetCaptchaAgain();
    void ConsoleLog(QString );
    void ForbiddenProxy();

    
public slots:
    void addVoice(QString num);
    void replyFinished(QNetworkReply*);
    void setProxy(QNetworkProxy p);

private slots:
    void sslErrors ( QNetworkReply * reply, const QList<QSslError> & errors );
    void ProxyIsDead();
    
};

#endif // TOPVOTER_H
