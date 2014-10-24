#ifndef _FULLCONNECT_H_
#define _FULLCONNECT_H_

#include <string>
#include <opencv2/opencv.hpp>
#include "common.h"
using namespace std;
using namespace cv;


class FullConnect
{
public:
	string name;
	int outputs;
	string neuron;

	int input_dim;

	Mat weights;
	double * biases;

	FullConnect();
	FullConnect(string name_input, int outputs_input, string neuron_input,int input_dim_input);

	int initialise(string model_savepath);

	int do_full_connect(Mat input, Mat & output_data);

	~FullConnect();
};

#endif