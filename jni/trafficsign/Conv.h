#include <string>
#include <opencv2/opencv.hpp>
#include "common.h"
#include "Non_linear_cvt.h"

using namespace std;
using namespace cv;

class Conv
{
public:
	string name;
	int filters;  //filter number
	int padding;
	int stride;
	int filter_size;
	int channels; 
	string neuron;

	Mat * *weights;
	double * biases;

	Conv(string name_input, int filters_input,int padding_input,int stride_input,int filter_size_input,int channels_input, string neuron_input);
	Conv();
	~Conv();


	int initialise(string model_save_path);
	int do_conv(Mat * input_img, Mat * output);

	int test();

private:
	int print_weights(int filter_index, int channel_index);
	int print_biaes();

	int nonlinear_cvt(Mat * img, int n, string type);

};