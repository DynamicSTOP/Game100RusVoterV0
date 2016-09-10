#ifndef TEST_H
#define TEST_H

#include <QtWebKit>
#include <QDebug>

class Test : QObject{
Q_OBJECT
public:
    Test(){
        view = new QWebView;
        view->load(QUrl("http://news.google.us"));
        connect(view, SIGNAL(loadFinished(bool)),this,SLOT(finishedLoading(bool)));
        //view->show();
    }
public slots:
    void finishedLoading(bool ok){
        QWebElementCollection elements = view->page()->mainFrame()->findAllElements("div");
        QList<QWebElement> list = elements.toList();
        qDebug() << ok << list.count();
}
private:
    QWebView *view;
};

#endif // TEST_H
