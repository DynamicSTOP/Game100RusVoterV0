#include "wincotroller.h"

WinCotroller::WinCotroller(QWidget *parent) :
    QWidget(parent)
{
    mainLayout = new QHBoxLayout(this);
    layoutButtons = new QVBoxLayout();
    mainLayout->addLayout(layoutButtons);

    log = new QTextEdit(this);
    log->setMinimumSize(300,400);
    log->setMaximumWidth(300);
    log->setMaximumHeight(400);
    log->setReadOnly(true);

    loadProxyList = new QPushButton(tr("load proxy list"),this);
    startWork = new QPushButton(tr("start work"),this);

    layoutButtons->addWidget(log);
    layoutButtons->addWidget(loadProxyList);
    layoutButtons->addWidget(startWork);

    layoutButtons->setAlignment(log,Qt::AlignTop);
    layoutButtons->setAlignment(loadProxyList,Qt::AlignTop);
    layoutButtons->setAlignment(startWork,Qt::AlignTop);

    mainLayout->setAlignment(layoutButtons,Qt::AlignLeft);
    mainLayout->addSpacing(640-log->width()-layoutButtons->margin()*2-mainLayout->margin()*2);

    QPalette palette;
    palette.setBrush(this->backgroundRole(), QBrush(QImage(":/ROHighWizard.png")));
    this->setPalette(palette);
    this->setWindowTitle(tr("Fuck the vote!"));

    this->setMinimumSize(640,512);
    imgHandler = new ImgHandler(this);
    connect(imgHandler,SIGNAL(ConsoleLog(QString)),log,SLOT(append(QString)));
    connect(startWork,SIGNAL(clicked()),imgHandler,SLOT(startWork()));
    connect(startWork,SIGNAL(clicked()),startWork,SLOT(hide()));
    connect(loadProxyList,SIGNAL(clicked()),imgHandler,SLOT(loadProxies()));
    connect(imgHandler,SIGNAL(ToggleWorkButton()),startWork,SLOT(show()));
}

