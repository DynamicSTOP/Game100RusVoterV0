#ifndef WINCOTROLLER_H
#define WINCOTROLLER_H

#include <QtGui>
#include <QtCore>
#include "imghandler.h"

class WinCotroller : public QWidget
{
    Q_OBJECT
public:
    explicit WinCotroller(QWidget *parent = 0);

private:
    ImgHandler *imgHandler;
    QHBoxLayout *mainLayout;

    QVBoxLayout *layoutButtons;


    QTextEdit *log;
    QPushButton *loadProxyList;
    QPushButton *startWork;

signals:
    

public slots:

    
};

#endif // WINCOTROLLER_H
