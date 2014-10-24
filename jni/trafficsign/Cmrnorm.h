#ifndef _CMRNORM_H_
#define _CMRNORM_H_

#include <string>
#include <opencv2/opencv.hpp>

#include "common.h"

using namespace std;
using namespace cv;

class Cmrnorm
{
public:
	string type;
	int channels, size;
	double scale, m_pow;

	int do_norm(Mat * input, Mat * output);
	Cmrnorm();
	Cmrnorm(string type_input, int channels_input, int size_input, double scale_input, double pow_input);
};

#endif