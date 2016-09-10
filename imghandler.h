#ifndef IMGHANDLER_H
#define IMGHANDLER_H
#include <QtCore>
#include <QtWebKit>
#include <QObject>
#include "QBitmap"
#include "renderer.h"
#include "captchbreaker.h"
#include "topvoter.h"

class ImgHandler : public QObject
{
    Q_OBJECT
public:
    explicit ImgHandler(QObject *parent = 0);
    
signals:
    void ConsoleLog(QString );
    void ToggleWorkButton();

public slots:
    void saveCaptcha(int threadId, QWebView *wv);
    void startWork();
    void solvedNumber(int position, int number);
    void setProxy(QNetworkProxy);
    void loadProxies();
    void goNextProxy();


private:
    TopVoter *voter;
    int solvedCount;
    QString solvedN;
    captchBreaker *cbr;
    Render *Renderer;

    QList<QNetworkProxy> proxyList;
    QStringList proxyListHosts;
    int attempts;
    int lastProxiId;
};

#endif // IMGHANDLER_H
