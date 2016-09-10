#ifndef RENDERER_H
#define RENDERER_H
#include <QtCore>
#include <QtWebKit>

class Render:public QObject{
    Q_OBJECT
public:
    Render();
    void getPage(QString);

    QWebView* getCapchaWebView();
    QString _url;
    void setNumber(int);
public slots:
    void captchaLoaded(bool);
    void loadCaptcha();
    void renderPictureCaptcha();

private:
    QWebView *wv_captcha;
    int number;

signals:
    void getPageDone();
    void sendReportCaptcha(int ,QWebView*);
};

#endif // RENDERER_H
