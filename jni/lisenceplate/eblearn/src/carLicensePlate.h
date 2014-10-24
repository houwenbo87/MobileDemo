#ifndef OCR_h
#define OCR_h

#include <string.h>
#include <vector>

#include "libeblearn.h"
#include "libeblearntools.h"
#include "eblapp.h"

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


#define HORIZONTAL    1
#define VERTICAL    0

class CharSegment{
public:
    CharSegment();
    CharSegment(Mat i, Rect p);
    Mat img;
    Rect pos;
};

class OCR{
public:
    bool DEBUG;
    bool saveSegments;
    string filename;
    static const int numCharacters;
    static const char strCharacters[];
    int onlychar_width;
    int onlychar_height;
    OCR(string trainFile);
    OCR();
    vector<Mat> run(Mat *input);
    int charSize;
    Mat preprocessChar(Mat in);
    int classify(Mat f);
    void train(Mat trainData, Mat trainClasses, int nlayers);
    Mat features(Mat input, int size);


    void sort_by_location(vector<CharSegment> *segments);
    //Mat	delete_edge(Mat IMG);
    Mat horizon_radon(Mat IMG_THRESNOLD);
    CvANN_MLP  ann;

    string str(vector<char>s);
private:
    bool trained;
    vector<CharSegment> segment(Mat input);
    Mat Preprocess(Mat in, int newSize);
    Mat getVisualHistogram(Mat *hist, int type);
    void drawVisualFeatures(Mat character, Mat hhist, Mat vhist, Mat lowData);
    Mat ProjectedHistogram(Mat img, int t);
    bool verifySizes(Mat r);

};

const std::string car_license_plate_labels[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
"A", "B", "C", "D", "E", "F", "G", "H", "J", "K", "L", "M", "N",
"P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

class carPlateDetect
{
public:
    carPlateDetect();
    ~carPlateDetect();

    configuration conf;
    detection_thread<float> *dt;
    camera<ubyte> *cam;
    OCR ocr;

    void init(const char* conf_fn);
    string detect(Mat &im);
};

#endif
