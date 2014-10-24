#ifndef _POOL_H_
#define  _POOL_H_

#include "common.h"
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;

class Pool
{
public:
	string pool;
	int sizeX, stride, channels;

	Pool(string pool_input, int sizeX_input, int stride_input, int channels_input);
	Pool();

	int do_pool(cv::Mat * img, cv::Mat * output);
};

#endif