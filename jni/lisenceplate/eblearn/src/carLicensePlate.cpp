#include "carLicensePlate.h"

int cnt = 0;

const int OCR::numCharacters = 34;
const char OCR::strCharacters[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };

CharSegment::CharSegment(){}
CharSegment::CharSegment(Mat i, Rect p){
    img = i;
    pos = p;
}

OCR::OCR(){
    DEBUG = false;
    trained = false;
    saveSegments = false;
    charSize = 20;
}

Mat OCR::preprocessChar(Mat in){
    //Remap image
    int h = in.rows;
    int w = in.cols;
    Mat transformMat = Mat::eye(2, 3, CV_32F);
    int m = max(w, h);
    transformMat.at<float>(0, 2) = m / 2 - w / 2;
    transformMat.at<float>(1, 2) = m / 2 - h / 2;

    Mat warpImage(m, m, in.type());
    warpAffine(in, warpImage, transformMat, warpImage.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(0));

    Mat out;
    resize(warpImage, out, Size(charSize, charSize));
    return out;
}

bool OCR::verifySizes(Mat r){
    float aspect = 45.0f / 90.0f;
    float charAspect = (float)r.cols / (float)r.rows;
    float error = 0.5;
    // 	float minHeight=onlychar_height/2;
    // 	float maxHeight=onlychar_height;
    float minHeight = 15;
    float maxHeight = 30;
    //We have a different aspect ratio for number 1, and it can be ~0.2
    float minAspect = 0.1;
    float maxAspect = aspect + aspect*error;
    //area of pixels
    float area = countNonZero(r);
    //bb area
    float bbArea = r.cols*r.rows;
    //% of pixel in area
    float percPixels = area / bbArea;

    if (DEBUG)
        cout << "Aspect: " << aspect << " [" << minAspect << "," << maxAspect << "] " << "Area " << percPixels << " Char aspect " << charAspect << " Height char " << r.rows << "\n";
    if (percPixels <= 1 && charAspect > minAspect && charAspect < maxAspect && r.rows >= minHeight && r.rows < maxHeight)
        return true;
    else
        return false;

}

vector<CharSegment> OCR::segment(Mat plate){
    Mat input = plate;
    vector<CharSegment> output;
    Mat img_threshold;
    threshold(input, img_threshold, 0, 255, CV_THRESH_OTSU);
    if (DEBUG)
        imshow("Threshold plate", img_threshold);

    onlychar_height = input.rows;
    Mat img_contours;
    img_threshold.copyTo(img_contours);
    vector< vector< Point> > contours;
    findContours(img_contours,
        contours, // a vector of contours
        CV_RETR_CCOMP, // retrieve the external contour
        CV_CHAIN_APPROX_NONE); // all pixels of each contours

    // Draw blue contours on a white image
    cv::Mat result;
    img_threshold.copyTo(result);
    cvtColor(result, result, CV_GRAY2RGB);
    cv::drawContours(result, contours,
        -1, // draw all contours
        cv::Scalar(255, 0, 0), // in blue
        1); // with a thickness of 1

    //Start to iterate to each contour founded
    vector<vector<Point> >::iterator itc = contours.begin();

    //Remove patch that are no inside limits of aspect ratio and area.    
    while (itc != contours.end())
    {
        Rect mr = boundingRect(Mat(*itc));
        rectangle(result, mr, Scalar(0, 255, 0));
        mr.x -= 1;
        mr.y -= 1;
        mr.width += 2;
        mr.height += 2;
        Mat auxRoi(img_threshold, mr);
        if (verifySizes(auxRoi)){
            auxRoi = preprocessChar(auxRoi);
            output.push_back(CharSegment(auxRoi, mr));
            //rectangle(result, mr, Scalar(0,125,255));
        }
        ++itc;
    }

    if (output.size() < 1)
    {
        return output;
    }
    sort_by_location(&output);
    for (int i = 0; i < output.size() - 1; i++)
    {
        int xx1 = max(output[i].pos.x, output[i + 1].pos.x);
        int yy1 = max(output[i].pos.y, output[i + 1].pos.y);
        int xx2 = min(output[i].pos.x + output[i].pos.width, output[i + 1].pos.x + output[i + 1].pos.width);
        int yy2 = min(output[i].pos.y + output[i].pos.height, output[i + 1].pos.y + output[i + 1].pos.height);
        int w = xx2 - xx1;
        int h = yy2 - yy1;
        if (w > 0 && h > 0)
        {
            double area = output[i + 1].pos.width*output[i + 1].pos.height;
            double confid = w * h / area;
            if (confid > 0.7)
            {
                vector<CharSegment>::iterator it = output.begin() + i + 1;
                output.erase(it);
                //j--;
            }
        }
    }

    if (DEBUG)
    {
        cout << "Num chars: " << output.size() << "\n";
        vector<CharSegment>::iterator its = output.begin();
        while (its != output.end())
        {
            Rect mr = its->pos;
            rectangle(result, mr, Scalar(125, 125, 255));
            ++its;
        }
    }
    if (DEBUG)
        imshow("SEgmented Chars", result);
    //waitKey(0);
    return output;
}

Mat OCR::ProjectedHistogram(Mat img, int t)
{
    int sz = (t) ? img.rows : img.cols;
    Mat mhist = Mat::zeros(1, sz, CV_32F);

    for (int j = 0; j < sz; j++){
        Mat data = (t) ? img.row(j) : img.col(j);
        mhist.at<float>(j) = countNonZero(data);
    }

    //Normalize histogram
    double min, max;
    minMaxLoc(mhist, &min, &max);

    if (max>0)
        mhist.convertTo(mhist, -1, 1.0f / max, 0);

    return mhist;
}



Mat OCR::features(Mat in, int sizeData){
    //Histogram features
    Mat vhist = ProjectedHistogram(in, VERTICAL);
    Mat hhist = ProjectedHistogram(in, HORIZONTAL);

    //Low data feature
    Mat lowData;
    resize(in, lowData, Size(sizeData, sizeData));

    // 		if(DEBUG)
    // 			drawVisualFeatures(in, hhist, vhist, lowData);

    //Last 10 is the number of moments components
    int numCols = vhist.cols + hhist.cols + lowData.cols*lowData.cols;

    Mat out = Mat::zeros(1, numCols, CV_32F);
    //Asign values to feature
    int j = 0;
    for (int i = 0; i < vhist.cols; i++)
    {
        out.at<float>(j) = vhist.at<float>(i);
        j++;
    }
    for (int i = 0; i < hhist.cols; i++)
    {
        out.at<float>(j) = hhist.at<float>(i);
        j++;
    }
    for (int x = 0; x < lowData.cols; x++)
    {
        for (int y = 0; y < lowData.rows; y++){
            out.at<float>(j) = (float)lowData.at<unsigned char>(x, y);
            j++;
        }
    }

    //     if(DEBUG)
    //         cout << out << "\n===========================================\n";
    return out;
}



int OCR::classify(Mat f){
    int result = -1;
    Mat output(1, numCharacters, CV_32FC1);
    ann.predict(f, output);
    Point maxLoc;
    double maxVal;
    minMaxLoc(output, 0, &maxVal, 0, &maxLoc);
    //We need know where in output is the max val, the x (cols) is the class.

    return maxLoc.x;
}


vector<Mat> OCR::run(Mat *input){

    //Segment chars of plate
    vector<CharSegment> segments = segment(*input);
    int segmentsize = segments.size();
    vector<Mat> chars;
    if (segmentsize < 1)
    {
        return chars;
    }

    for (int i = 0; i < segments.size(); i++)
    {
        Mat ch = preprocessChar(segments[i].img);
        Mat f = features(ch, 15);
        Mat save_characters((*input), segments[i].pos);
#if 0
        int character = 0;
        string savname = format("D:\\st\\plate\\test\\%d_%d.jpg", character, cnt);
        cnt++;
        imwrite(savname, save_characters);
#endif
        chars.push_back(save_characters);
    }

    return chars;
}


//Mat OCR::delete_edge(Mat IMG)
//{
//    bool lab = false;
//    bool change = false;
//    int rect_up = 0;
//    int rect_down = 0;
//    int imgTop;
//    int imgBottom;
//    for (int wt = 0; wt < IMG.rows; wt++)
//    {
//        int count = 0;
//        for (int ht = 0; ht < IMG.cols; ht++)
//        {
//            if ((255 == IMG.at<uchar>(wt, ht)) && (!change))
//            {
//                count++;
//                change = true;
//            }
//            else if ((0 == IMG.at<uchar>(wt, ht)) && (change))
//            {
//                count++;
//                change = false;
//            }
//        }
//        if ((count>8) && !lab)
//        {
//            if (wt > 3)
//                rect_up = wt - 3;
//            else rect_up = wt;
//            lab = true;
//        }
//        if ((count < 8) && lab)
//        {
//            rect_down = wt + 2;
//            lab = false;
//        }
//        if (rect_down - rect_up<IMG.cols && rect_down - rect_up >IMG.cols / 4)
//        {
//            imgTop = rect_up;
//            imgBottom = rect_down;
//            break;
//        }
//    }
//    int s_height = imgBottom - imgTop;
//    Mat fg = IMG(Rect(0, imgTop, IMG.cols, s_height));
//    threshold(fg, fg, 0, 255, CV_THRESH_OTSU);
//    // 	imshow("video7",fg);
//    // 	waitKey(0);
//    return fg;
//}

Mat OCR::horizon_radon(Mat img_threshold)
{
    int i, j;
    int nmax, Max;
    int theta, thro;
    int **count;
    double fSinAngle, fCosAngle;
    int Height = (img_threshold).rows;
    int Width = (img_threshold).cols;
    const int MaxTheta = 40;
    const int MinTheta = -40;
    int dValue[MaxTheta - MinTheta + 1];
    memset(dValue, 0, sizeof(int)*(MaxTheta - MinTheta + 1));
    nmax = (int)sqrt(double(Height*Height + Width*Width));
    count = new int *[MaxTheta - MinTheta + 1];
    for (theta = 0; theta < MaxTheta - MinTheta + 1; theta++)  //��0
    {
        count[theta] = new int[nmax];
        memset(count[theta], 0, sizeof(int)*nmax);
    }
    for (theta = MinTheta; theta < MaxTheta; theta++)
    {
        fSinAngle = sin(CV_PI*theta / 180);
        fCosAngle = cos(CV_PI*theta / 180);
        for (i = 0; i < Height; i++)
        {
            for (j = 0; j < Width; j++)
            {
                if (img_threshold.at<uchar>(i, j) == 255)

                {
                    thro = (i*fCosAngle + j*fSinAngle);
                    if (thro>0 && thro < nmax)
                    {   //theta+20
                        count[theta + 40][thro]++;
                    }
                }

            }
        }
    }

    for (i = 0; i < (MaxTheta - MinTheta + 1); i++)
        for (j = 0; j < nmax; j++)
        {
        dValue[i] += fabs(double(count[i][j] - count[i][j + 1]));
        }
    Max = dValue[0];
    for (i = 0; i<(MaxTheta - MinTheta + 1); i++)
        if (dValue[i]>Max)
        {
        Max = dValue[i];
        theta = i - 40;
        }
    cv::Point2f center = cv::Point2f(img_threshold.cols / 2, img_threshold.rows / 2);
    Mat rotmat = getRotationMatrix2D(center, -1 * theta, 1);
    Size new_rotate(img_threshold.cols, img_threshold.rows);

    //Create and rotate image
    Mat img_rotated;
    warpAffine(img_threshold, img_rotated, rotmat, new_rotate, CV_INTER_CUBIC);

    //imshow("Timg_rotated", img_rotated);
    //waitKey(0);
    return img_rotated;

}

void OCR::sort_by_location(vector<CharSegment>*fg)
{
    CharSegment temp;

    int s = (*fg).size();

    for (int i = 0; i < s; i++)
    {
        for (int j = 0; j < s - i - 1; j++)
        {
            if ((*fg)[j].pos.x > (*fg)[j + 1].pos.x)
            {
                temp = (*fg)[j];
                (*fg)[j] = (*fg)[j + 1];
                (*fg)[j + 1] = temp;
            }
        }
    }

}


string OCR::str(vector<char>s)
{
    string result = "";
    for (int i = 0; i < s.size(); i++){
        result = result + s[i];
    }
    return result;
}

carPlateDetect::carPlateDetect()
{
    dt = NULL;
    cam = NULL;
}

carPlateDetect::~carPlateDetect()
{
    if (cam)
        delete cam;
    if (dt)
    {
        dt->destroyDetectionThread();
        delete dt;
    }
}

void carPlateDetect::init(const char* conf_fn)
{
    // load configuration
    conf.init(conf_fn, true, true, false);
    if (!conf.exists("root2") || !conf.exists("current_dir"))
    {
        string dir;
        dir << dirname(conf_fn) << "/";
        conf.set("root2", dir.c_str());
        conf.set("current_dir", dir.c_str());
    }
    conf.set("run_type", "detect"); // tell conf that we are in detect mode
    conf.resolve(); // manual call to resolving variable
    bool silent = conf.exists_true("silent");
    if (conf.exists_true("show_conf") && !silent)
        conf.pretty();

    dt = new detection_thread<float>(conf);
    dt->initDetectionThread();

    // initialize camera (opencv, directory, shmem or video)
    cam = NULL;
    {
        string dir;
        if (conf.exists("input_dir"))
            dir = conf.get_string("input_dir");
        list<string> files;
        cam = new camera_directory<ubyte>(dir.c_str(), -1, -1, false, 1, cout, cerr, IMAGE_PATTERN_MAT, &files);
    }
    if (conf.exists_true("silent"))
        cam->set_silent();

}

string carPlateDetect::detect(Mat &img)
{
	Mat im;
	resize(img, im, Size(img.cols * 40 / img.rows, 40));
    vector<Mat> plateImages = ocr.run(&im);

    string plateNumber;
    for (size_t i = 1; i < plateImages.size(); i++)
    {
        Size dsize = Size(28, 28);
        Mat img_resize;
        resize(plateImages[i], img_resize, dsize);
        string filename = format("/storage/emulated/0/lisenceplate/plateimage_%d.jpg", i);
        imwrite(filename, img_resize);
        dt->execute(img_resize);
        bboxes bb;
        uint total_saved = 0;
        dt->get_data(bb, total_saved);
        plateNumber += car_license_plate_labels[bb[0].class_id];
    }

    return plateNumber;
}
