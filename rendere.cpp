#include <renderer.h>
#include "QBitmap"

Render::Render(){
    this->number=0;
    wv_captcha=0;
}

void Render::setNumber(int n){
    this->number=n;
}

void Render::getPage(QString url){
    _url.clear();
    _url.append(url);

    if(wv_captcha)wv_captcha->deleteLater();
    wv_captcha = new QWebView();

    wv_captcha->settings()->setAttribute(QWebSettings::AutoLoadImages, false);
    wv_captcha->settings()->setAttribute(QWebSettings::JavascriptEnabled,true);
    wv_captcha->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    wv_captcha->settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls,true);
    wv_captcha->settings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls,true);
    wv_captcha->settings()->setAttribute(QWebSettings::XSSAuditingEnabled,true);
    wv_captcha->settings()->setAttribute(QWebSettings::SiteSpecificQuirksEnabled,true);

    wv_captcha->setBackgroundRole(QPalette::NoRole);
    wv_captcha->setStyleSheet("background:white;");

    QObject::connect(wv_captcha,SIGNAL(loadFinished(bool)),this,SLOT(captchaLoaded(bool)));
    loadCaptcha();
}

void Render::loadCaptcha(){
    wv_captcha->load(QString(_url.toLocal8Bit() + "icaptcha.swf?ok=Correct&wrong=Wrong%20Code&c1=1&c2=1&c3=0&c4=1&c5=1&mar=0&gro=3&vel=50&vim=100&leng=php&color=1"));
}

void Render::captchaLoaded(bool b){
    if(b){
        QWebElementCollection elements = wv_captcha->page()->mainFrame()->findAllElements(tr("body"));
        wv_captcha->page()->mainFrame()->evaluateJavaScript(tr("var a = document.getElementsByTagName('embed')[0];\
                                                               a.setAttribute('wmode','transparent');\
                                                               var newDiv = document.createElement('div');\
                                                               var bod = document.getElementsByTagName('body')[0];\
                                                               bod.setAttribute('bgcolor','rgba(255,0,0,0)');\
                                                               bod.appendChild(newDiv);\
                                                               newDiv.appendChild(a);"
                                                               ));
        QTimer::singleShot(5000,this,SLOT(renderPictureCaptcha()));
    } else {
        qWarning()<<"error on load capcha finished";
    }
}

void Render::renderPictureCaptcha(){
    emit sendReportCaptcha(number, wv_captcha);
}

QWebView* Render::getCapchaWebView(){
    return wv_captcha;
}
