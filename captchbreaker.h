#ifndef CAPTCHBREAKER_H
#define CAPTCHBREAKER_H

#include <QtCore>
#include <QObject>
#include <QPixmap>

struct NeuronNetNumber{
    const static int sizeX = 127;
    const static int sizeY = 172;
    float data[NeuronNetNumber::sizeX][NeuronNetNumber::sizeY];
    int dataCount;
    int number;

    static NeuronNetNumber create(){
        NeuronNetNumber n;
        n.number=0;
        n.dataCount=0;
        for(int x=0;x<n.sizeX;x++)
            for(int y=0;y<n.sizeY;y++)
                n.data[x][y]=0;
        return n;
    }
};

Q_DECLARE_METATYPE( NeuronNetNumber )
typedef QList<NeuronNetNumber> NeuronNetNumbers;

QDataStream & operator<< ( QDataStream & stream, const NeuronNetNumber & vs );
QDataStream & operator>> ( QDataStream & stream, NeuronNetNumber & vs );


class checker : public QObject {
    //for multithreading. this class would only try to get results faster
    Q_OBJECT
    public:
        explicit checker(QObject *parent=0):QObject(parent){currentCheckerNumber=0;}
        void setCheckerNumber(int n){
            currentCheckerNumber=n;
        }

        /*
        Mat rotateMat(Mat source, int angle,int width,int height, bool save=false);
        static Mat splitElement(Mat source, int elementNumber);
        static Mat qimage2mat(const QImage& qimage);
        */
    private:
        int currentCheckerNumber;
    public slots:
        void    check(QImage,int,NeuronNetNumbers neuronsData);
        float   checkDots(QImage,int num,NeuronNetNumbers NeuronData);
        /*float   checkDots(Mat,int num,NeuronNetNumbers NeuronData);*/
        /*void    checkMat(QImage,int,NeuronNetNumbers neuronsData);*/
    signals:
        void    checkResult(int position,int number,int angle);
        void    solvedNumber(int,int);
};



class captchBreaker : public QObject
{
    Q_OBJECT
public:

    NeuronNetNumbers NeuronData;
    QList<checker*> Checkers;
    void generateNetDataFile();
    void loadNetDataFile();
    void saveNetDataFile();
    void renderExperience();
    /*void renderExperienceMat();*/

    void addDataToNeuronNet(int neuronCellNumber, QImage img);
    /*void addDataToNeuronNetMat(int neuronCellNumber, QImage source, int el, int angle);*/


    explicit captchBreaker(QObject *parent = 0);
    int number;

    void solve(QPixmap*);

    void solvePartNumber(QImage, int el);
    void solvePartNumber(QPixmap *, int el);

    QString parseCaptch(QString path);
    QImage fixBlack(QImage img);

    static QImage rotatedImage(int angle, QImage i);
    static QImage rotatedImage(int angle, QPixmap p);

    static QPixmap getElement(QPixmap*, int elNumber, int angle);
    static QImage getElement(QImage, int elNumber, int angle);

    int checkImage(QImage img);

signals:
    void solved(int position,int number,int angle);
    void jobAdd(QImage img, int el, NeuronNetNumbers );
    void solvedNumber(int,int);
    void ConsoleLog(QString);
public slots:



    
};

#endif // CAPTCHBREAKER_H
