#include <iostream>
#include <fstream>
#include <cv.h>

using namespace std;
using namespace cv;

class Img2IPM
{
public:
	int process(Point pt[4], string save_path, int img_cols, int img_rows);
};