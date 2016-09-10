#include "captchbreaker.h"
#include "QPixmap"
#include "QBitmap"
#include "QPainter"
#include <QtCore>

captchBreaker::captchBreaker(QObject *parent) : QObject(parent){


    loadNetDataFile();
    /*renderExperienceMat();*/
    renderExperience();
    qRegisterMetaType<NeuronNetNumbers>("NeuronNetNumbers");

    for(int i = 0; i<5;i++){
        checker *c = new checker();
        Checkers.append(c);
        c->setCheckerNumber(i);
        QThread *t = new  QThread();
        t->start();
        while(!t->isRunning()){}
        c->moveToThread(t);

        /*connect(this,SIGNAL(jobAdd(QImage,int,NeuronNetNumbers)),
                c,SLOT(checkMat(QImage,int,NeuronNetNumbers)));*/
        connect(this,SIGNAL(jobAdd(QImage,int,NeuronNetNumbers)),
                        c,SLOT(check(QImage,int,NeuronNetNumbers)));
        connect(c,SIGNAL(checkResult(int,int,int)),this,SIGNAL(solved(int,int,int)));
        connect(c,SIGNAL(solvedNumber(int,int)),this,SIGNAL(solvedNumber(int,int)));

    }
}

QString captchBreaker::parseCaptch(QString path){
}


QImage captchBreaker::rotatedImage(int angle, QImage i){
    QImage image(i);
    QMatrix rm;
    rm.rotate(angle);
    image = image.transformed(rm,Qt::SmoothTransformation);
    image = image.copy((image.width()-i.width())/2+60,(image.height()-i.height())/2,i.width()-120,i.height() );

    return image;
}

QImage captchBreaker::rotatedImage(int angle, QPixmap p){
    return rotatedImage(angle, p.toImage());

}

QImage captchBreaker::fixBlack(QImage img){
    QImage image(img);
    for(int x=0;x<img.width();x++)
        for(int y=0;y<img.height();y++){
            if(QColor::fromRgba(img.pixel(x,y)).alpha()!=255){
                image.setPixel(x,y,qRgba(255,255,255,255));
            }else if(qGray(img.pixel(x,y))<200)
                image.setPixel(x,y,qRgb(0,0,0));
        }
    return image;
}

QPixmap captchBreaker::getElement(QPixmap* p,int num, int angle){
    int width = p->width()/5;
    QPixmap r(QSize(width+120,p->height()));
    r.fill(Qt::white);
    if(num == 0 ){//start add right
        QPainter painter(&r);
        painter.drawPixmap(QPoint(60,0),p->copy(0,0,width+60,p->height()));
        painter.end();
    }
    else if(num==4){//end add right
        r.fill(Qt::white);
        QPainter painter(&r);
        painter.drawPixmap(QPoint(0,0),p->copy(num*width-60,0,width+60,p->height()));
        painter.end();
    } else {
        r = p->copy(num*width-60,0,width+120,p->height());
    }

    r = QPixmap::fromImage(rotatedImage(angle, r));

    return r;
}

QImage captchBreaker::getElement(QImage i,int num, int angle){
    int width = i.width()/5;
    QImage r(QSize(width+120,i.height()),QImage::Format_ARGB32);
    r.fill(Qt::white);
    if(num == 0 ){//start add right
        QPainter painter(&r);
        painter.drawImage(QPoint(60,0),i.copy(0,0,width+60,i.height()));
        painter.end();
    }
    else if(num==4){//end add right
        r.fill(Qt::white);
        QPainter painter(&r);
        painter.drawImage(QPoint(0,0),i.copy(num*width-60,0,width+60,i.height()));
        painter.end();
    } else {
        r = i.copy(num*width-60,0,width+120,i.height());
    }

    r = rotatedImage(angle, r);

    return r;
}

void captchBreaker::solvePartNumber(QPixmap* pixmap,int el){
    solvePartNumber(pixmap->toImage(),el);
}

void captchBreaker::solvePartNumber(QImage img,int el){
    emit jobAdd(img,el,NeuronData);
}

void captchBreaker::solve(QPixmap* origin){
    QString answer;
    for(int el =0 ;el<5; el++){
        solvePartNumber(origin,el);
    }
}

/************************************************************************************************************************************************************/

QDataStream & operator<< ( QDataStream & stream, const NeuronNetNumber & nnn ){
    stream<<nnn.number;
    stream<<nnn.dataCount;
    for(int x=0;x<nnn.sizeX;x++)
        for(int y=0;y<nnn.sizeY;y++)
            stream<<nnn.data[x][y];

    return stream;
}

QDataStream & operator>> ( QDataStream & stream, NeuronNetNumber & nnn ){
    stream>>nnn.number;
    stream>>nnn.dataCount;
    for(int x=0;x<nnn.sizeX;x++)
        for(int y=0;y<nnn.sizeY;y++){
            stream>>nnn.data[x][y];
        }

    return stream;
}

void captchBreaker::generateNetDataFile(){
    NeuronData.clear();
    for(int i=0;i<10;i++){
        NeuronNetNumber n= NeuronNetNumber::create();
        n.number=i;
        NeuronData.append(n);
    }
    saveNetDataFile();
}

void captchBreaker::loadNetDataFile(){
    QFile neuronDataFile("ndata.dat");
    if(!neuronDataFile.exists())
        generateNetDataFile();

    NeuronData.clear();

    neuronDataFile.open(QIODevice::ReadOnly);
    QDataStream str(&neuronDataFile);
    str>>NeuronData;
    neuronDataFile.close();
}

void captchBreaker::renderExperience(){
    QImage img((NeuronNetNumber::sizeX+10)*5,(NeuronNetNumber::sizeY)*2+10,QImage::Format_ARGB32);
    img.fill(Qt::black);
    int offsetX=0,offsetY=(NeuronNetNumber::sizeY)+10;

    for(int n = 0; n < 5; n++){
        NeuronNetNumber Neuron = NeuronData[n];

        offsetX=(NeuronNetNumber::sizeX+10)*n;
        for(int x = 0; x<NeuronNetNumber::sizeX; x++)
            for(int y = 0; y<NeuronNetNumber::sizeY; y++){
                img.setPixel(offsetX+x,y,qRgb(0,255.0*Neuron.data[x][y],0));
            }
    }

    for(int n = 5; n < 10; n++){
        NeuronNetNumber Neuron = NeuronData[n];
        offsetX=(NeuronNetNumber::sizeX+10)*(n-5);

        for(int x = 0; x<NeuronNetNumber::sizeX; x++)
            for(int y = 0; y<NeuronNetNumber::sizeY; y++){
                img.setPixel(offsetX+x,offsetY+y,qRgb(0,255.0*Neuron.data[x][y],0));
            }
    }
    img.save("memory.png");
}
/*
void captchBreaker::renderExperienceMat(){
    Mat img = Mat::zeros((NeuronNetNumber::sizeY)*2+10,(NeuronNetNumber::sizeX+10)*5,CV_8UC3);
    int offsetX=0,offsetY=(NeuronNetNumber::sizeY)+10;

    for(int n = 0; n < 5; n++){
        NeuronNetNumber Neuron = NeuronData[n];

        offsetX=(NeuronNetNumber::sizeX+10)*n;
        for(int x = 0; x<NeuronNetNumber::sizeX; x++)
            for(int y = 0; y<NeuronNetNumber::sizeY; y++){
                img.at<Vec3b>(y,offsetX+x)[1]=255.0*Neuron.data[x][y];
            }
    }

    for(int n = 5; n < 10; n++){
        NeuronNetNumber Neuron = NeuronData[n];
        offsetX=(NeuronNetNumber::sizeX+10)*(n-5);

        for(int x = 0; x<NeuronNetNumber::sizeX; x++)
            for(int y = 0; y<NeuronNetNumber::sizeY; y++){
                img.at<Vec3b>(offsetY+y,offsetX+x)[1]=255.0*Neuron.data[x][y];
            }
    }
    imwrite("memory.png",img);
}
*/

void captchBreaker::saveNetDataFile(){
    QFile neuronDataFile("ndata.dat");
    neuronDataFile.open(QIODevice::WriteOnly);
    neuronDataFile.resize(0);
    QDataStream str(&neuronDataFile);
    str<<NeuronData;
    neuronDataFile.close();
    /*renderExperienceMat();*/
    renderExperience();
}

void captchBreaker::addDataToNeuronNet(int neuronCellNumber, QImage img){

    NeuronNetNumber Neuron = NeuronData[neuronCellNumber];
    if(img.size()!=QSize(Neuron.sizeX,Neuron.sizeY)){
        qWarning()<<"size problem"<<img.size()<<Neuron.sizeX<<Neuron.sizeY;
        img.save("pr.png");
        return;
    }

    for(int x=0;x<Neuron.sizeX;x++)
        for(int y=0;y<Neuron.sizeY;y++){
            if(QColor::fromRgba(img.pixel(x,y)).alpha()!=255){//if it's transparent
                Neuron.data[x][y] = (float)(Neuron.data[x][y]*Neuron.dataCount)/(Neuron.dataCount+1.0);
            }else if(qGray(img.pixel(x,y))<200){//if it's more likely black then white
                if(Neuron.dataCount>100 && Neuron.data[x][y]>0.7)//if we haven't got there black wery often
                    Neuron.data[x][y] = (float)(Neuron.data[x][y]*Neuron.dataCount+1.0)/(Neuron.dataCount+1.0);
                else if(Neuron.dataCount<=100)//if we haven't got there black wery often
                    Neuron.data[x][y] = (float)(Neuron.data[x][y]*Neuron.dataCount+1.0)/(Neuron.dataCount+1.0);
                else
                    Neuron.data[x][y] = (float)(Neuron.data[x][y]*Neuron.dataCount)/(Neuron.dataCount+1.0);
            } else {//if it's more likely white then black
                Neuron.data[x][y] = (float)(Neuron.data[x][y]*Neuron.dataCount)/(Neuron.dataCount+1.0);
            }
        }

    Neuron.dataCount++;
    NeuronData[neuronCellNumber]=Neuron;
}

/*
void captchBreaker::addDataToNeuronNetMat(int neuronCellNumber, QImage img, int el, int angle){
    NeuronNetNumber Neuron = NeuronData[neuronCellNumber];

    angle=angle*-1;

    Mat source = Checkers.at(el)->qimage2mat(img);
    int width = source.cols/5, height = source.rows;
    Mat imagePart = Checkers.at(el)->splitElement(source, el);

    //Mat checker::rotateMat(Mat mat, int angle, int width, int height, bool save){
    //Mat rotated = Checkers.at(el)->rotateMat(imagePart,angle,width,height,true);

    Mat rot_mat = getRotationMatrix2D( Point(imagePart.cols/2, imagePart.rows/2), angle, 1.0);
    /// Rotate the warped image
    Mat rotatedA,rotated;
    warpAffine( imagePart, rotatedA , rot_mat, imagePart.size() );
    rotatedA = rotatedA(Range(rotatedA.rows/2-height/2,rotatedA.rows/2+height/2),Range(rotatedA.cols/2-width/2,rotatedA.cols/2+width/2));

    cvtColor(rotatedA,rotated,CV_GRAY2RGB);

    for(int x=0;x<Neuron.sizeX;x++){
        for(int y=0;y<Neuron.sizeY;y++){
            if(((int)(rotated.at<Vec3b>(y,x)[0]))<200 ){//if it's more likely black then white
                if((Neuron.dataCount>100 && Neuron.data[x][y]>0.7)//if we haven't got there black wery often
                        || Neuron.dataCount<=100){//or if we haven't got there black wery often
                    Neuron.data[x][y] = (float)(Neuron.data[x][y]*Neuron.dataCount+1.0)/(Neuron.dataCount+1.0);
                } else {
                    Neuron.data[x][y] = (float)(Neuron.data[x][y]*Neuron.dataCount)/(Neuron.dataCount+1.0);
                }
            } else {//if it's more likely white then black
                Neuron.data[x][y] = (float)(Neuron.data[x][y]*Neuron.dataCount)/(Neuron.dataCount+1.0);
            }
        }
    }

    Neuron.dataCount++;
    NeuronData[neuronCellNumber]=Neuron;
}
*/
/************************************************************************************************************************************************************/

void checker::check(QImage img, int el, NeuronNetNumbers neuronsData){
    if(el!=currentCheckerNumber)
        return;
    float max_chance=0,num=0;
    int angleOfMaxChance=0;
    for(int Number=0;Number<10;Number++){
        float cur_chance=0;
        float temp_max=0;
        for(int a=-50;a<=50;a++){
            cur_chance = checkDots(captchBreaker::getElement(img,el,a),Number,neuronsData);
            if(cur_chance>max_chance){
                max_chance=cur_chance;
                angleOfMaxChance=a;
                num=Number;
            }//chance check
            if(cur_chance>temp_max){
                temp_max=cur_chance;
            }
        }//for angle
        //qWarning()<<el<<Number<<temp_max;
    }//for numbers

    qWarning()  <<QTime::currentTime()<<"\tel"<<el
                <<"\tnumber"<<num
                <<"\tangle"<<angleOfMaxChance
                <<"\tMchance"<<max_chance;
    emit checkResult(el,num,angleOfMaxChance);
    emit solvedNumber(el,num);
}

float checker::checkDots(QImage img,int num, NeuronNetNumbers NeuronData){
    float result = 0;
    NeuronNetNumber Neuron = NeuronData[num];

    for(int x = 0; x < NeuronNetNumber::sizeX; x++)
        for(int y = 0; y < NeuronNetNumber::sizeY; y++){

            if(Neuron.data[x][y]>0.6 && QColor::fromRgba(img.pixel(x,y)).alpha()==255 && qGray(img.pixel(x,y))<200 ){//if it's more likely black then white
                //if neuron data is high
                //pixel isn't transparent and more likely black
              result += Neuron.data[x][y];
            } else if(Neuron.data[x][y] <= 0.6 && (QColor::fromRgba(img.pixel(x,y)).alpha()!=255 || qGray(img.pixel(x,y))>=200)){
                //else if it's transparent or more  likely white
                //and neuron data is low
                result += 1.0 - Neuron.data[x][y];
            } else {
                result -= Neuron.data[x][y];
            }
        }
    return result;
}

/*
Mat checker::qimage2mat(const QImage& qimage){
    cv::Mat mat = cv::Mat(qimage.height(), qimage.width(), CV_8UC4, (uchar*)qimage.bits(), qimage.bytesPerLine());
    cv::Mat mat2 = cv::Mat(mat.rows, mat.cols, CV_8UC3 );
    int from_to[] = { 0,0,  1,1,  2,2 };
    cv::mixChannels( &mat, 1, &mat2, 1, from_to, 3 );
    return mat2;
}

Mat checker::splitElement(Mat source, int elementNumber){
    int width = source.cols/5, width_total = width+120;
    Range c,r=Range(0,source.rows);
    int RBorder = width_total-r.size();

    if(elementNumber==0)
        c = Range(0,width_total-60);
    else if(elementNumber==4)
        c = Range(source.cols-width-60, source.cols);
    else
        c = Range(width*elementNumber-60, width*elementNumber+width+60);

    Mat NumberSequence = source.colRange(c).rowRange(r);


    if(elementNumber==0)
        copyMakeBorder(NumberSequence,NumberSequence,RBorder/2,RBorder/2,60,0,BORDER_CONSTANT,Scalar(255,255,255));
    else if(elementNumber==4)
        copyMakeBorder(NumberSequence,NumberSequence,RBorder/2,RBorder/2,0,60,BORDER_CONSTANT,Scalar(255,255,255));
    else
        copyMakeBorder(NumberSequence,NumberSequence,RBorder/2,RBorder/2,0,0,BORDER_CONSTANT,Scalar(255,255,255));

    cvtColor(NumberSequence,NumberSequence,CV_RGB2GRAY);

    return NumberSequence;
}

Mat checker::rotateMat(Mat mat, int angle, int width, int height, bool save){
    Mat rot_mat = getRotationMatrix2D( Point(mat.cols/2,mat.rows/2), angle, 1.0);
    /// Rotate the warped image
    Mat rotated;
    warpAffine( mat, rotated , rot_mat, mat.size() );
    Mat cropped = rotated(Range(rotated.rows/2-height/2,rotated.rows/2+height/2),Range(rotated.cols/2-width/2,rotated.cols/2+width/2));

    if(save){
        char str[12];
        sprintf(str, "%d__.png", angle);
        imwrite(str,cropped);
    }
    return cropped;
}


float checker::checkDots(Mat rotA,int num, NeuronNetNumbers NeuronData){
    float result = 0;
    NeuronNetNumber Neuron = NeuronData[num];
    if(Neuron.dataCount==0)
        return result;
    Mat img;
    cvtColor(rotA,img,CV_GRAY2RGB);

    for(int x = 0; x < NeuronNetNumber::sizeX; x++)
        for(int y = 0; y < NeuronNetNumber::sizeY; y++){
            if(Neuron.data[x][y]>0.7F && ((int)(img.at<Vec3b>(y,x)[1]))<200 ){//if it's more likely black then white
                //if neuron data is high
                //pixel isn't transparent and more likely black
              result += Neuron.data[x][y];
            } else if(Neuron.data[x][y] <= 0.7F && ((int)(img.at<Vec3b>(y,x)[1]))>=200){
                //else if it's transparent or more  likely white
                //and neuron data is low
                result += 1.0 - Neuron.data[x][y];
            } else {
                result -= Neuron.data[x][y];
            }
        }
    return result;
}

void checker::checkMat(QImage img, int el, NeuronNetNumbers neuronsData){
    if(el!=currentCheckerNumber)
        return;

    Mat source = qimage2mat(img);
    int width = source.cols/5, height = source.rows;
    Mat imagePart = splitElement(source,el);

    float max_chance=0,num=0;
    int angleOfMaxChance=0;
    for(int Number=0;Number<10;Number++){
        float cur_chance=0;
        float temp_max=0;
        for(int a=-50;a<=50;a++){
            cur_chance = checkDots(rotateMat(imagePart,a,width,height),Number,neuronsData);
            if(cur_chance>max_chance){
                max_chance=cur_chance;
                angleOfMaxChance=a;
                num=Number;
            }//chance check
            if(cur_chance>temp_max){
                temp_max=cur_chance;
            }
        }//for angle
    }
    angleOfMaxChance= angleOfMaxChance*-1;
    emit checkResult(el,num,angleOfMaxChance);
}
*/
