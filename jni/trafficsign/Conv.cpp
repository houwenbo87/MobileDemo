
#include <fstream>
#include "Conv.h"
#include <opencv2/opencv.hpp>
#include <math.h>
using namespace std;
using namespace cv;


Conv::Conv(string name_input, int filters_input,int padding_input,int stride_input,int filter_size_input,int channels_input, string neuron_input)
{
	name = name_input;
	filters = filters_input;
	padding = padding_input;
	stride = stride_input;
	filter_size = filter_size_input;
	channels = channels_input;
	neuron = neuron_input;

	weights = NULL;
	biases = NULL;
}


int Conv::initialise(string model_save_path)
{

	// load weights
	//int dims[] = {filter_size, filter_size, channels};
	weights = new Mat * [filters];
	if (weights == NULL)
	{
		cout<<"ERROR: malloc memory fails"<<endl;
		return ERROR_MALLOC_MEMORY;
	}

	for (int i = 0; i < filters; i++)
	{
		weights[i] = new Mat[channels];
		if (weights[i] == NULL)
		{
			cout<<"ERROR: malloc memory fails"<<endl;
			return ERROR_MALLOC_MEMORY;
		}
	}

	for (int i = 0; i < filters; i++)
	{
		for (int j = 0; j < channels; j++)
		{
			weights[i][j].create(filter_size,filter_size, CV_64FC1);
		}
	}
	

	for (int i = 0; i < filters; i++)
	{
		char input_filename[500];
		sprintf(input_filename, (model_save_path + name + "_weights_%d.txt").c_str(), i);
		ifstream input_file(input_filename);

		if (input_file.fail()) { cout<<"ERROR: cannot open model file "<<input_filename<<endl; return ERROR_INITIAL_CNN_MODEL; };

		double tmp_double = 0;
		for (int channel_index = 0; channel_index < channels; channel_index++)
		{
			Mat tmp_weight = weights[i][channel_index];
			for (int row_index = 0; row_index < filter_size; row_index++)
			{
				double * tmp_weight_ptr = tmp_weight.ptr<double>(row_index);
				for (int col_index = 0; col_index < filter_size; col_index++)
				{
					//cout<<"i "<<row_index<<" j"<<col_index<<endl;
					
					input_file>>tmp_double;
					tmp_weight_ptr[col_index] = tmp_double;
				}
			}
		}

		input_file.close();
	}

	//load biases
	biases = new double[filters];
	if (biases == NULL)
	{
		cout<<"ERROR: malloc memory fails"<<endl;
		return ERROR_MALLOC_MEMORY;
	}
	string input_filename = model_save_path + name + "_biases.txt";
	ifstream input_file(input_filename.c_str());

	if (input_file.fail())  { cout<<"ERROR: cannot open model file "<<input_filename<<endl;  return ERROR_INITIAL_CNN_MODEL;}

	for (int i = 0; i < filters; i++)
	{
		input_file>>biases[i];
	}
	input_file.close();

	return 0;
}

int Conv::do_conv(Mat * input_data, Mat * output)
{
	// output 是在外面申请好的，由外面管理内存

	int ret_code = 0;

	int img_rows = input_data[0].rows;

	Mat * input_img = new Mat[channels];
	if (input_img == NULL) { return ERROR_MALLOC_MEMORY;}

	// input_img边界扩展，加0, input_img变大
	for (int i = 0; i < channels; i++)
	{
		copyMakeBorder(input_data[i],input_img[i], padding,padding, padding, padding, BORDER_CONSTANT,0);
	}

	Mat tmp(img_rows, img_rows, CV_64FC1, Scalar::all(0));
	Mat single_channel_filter_result;
	for (int filter_index = 0; filter_index < filters; filter_index++)
	{
		tmp.setTo(0);
		// imfilter
		for (int channel_index = 0; channel_index < channels; channel_index++)
		{
			filter2D(input_img[channel_index], single_channel_filter_result, CV_64F, weights[filter_index][channel_index]);
			tmp += single_channel_filter_result(Rect(padding, padding, img_rows, img_rows));
		}


		// add biases
		tmp += biases[filter_index];
		//output[filter_index].release();
		tmp.copyTo(output[filter_index]);
	}
	
	

	//non-linear cvt
	ret_code = nonlinear_cvt(output, filters, neuron);
	//cout<<"ok"<<endl;

	if (input_img != NULL)
	{
		delete[] input_img;
	}
	return ret_code;
}

int Conv::test()
{
	int filter_index = 5;
	int channel_index = 1;
	print_weights(filter_index, channel_index);

	print_biaes();


	return 0;
}

int Conv::print_weights(int filter_index, int channel_index)
{
	cout<<name<<"  weight "<<filter_index<<"  "<<channel_index<<endl;
	Mat weight = weights[filter_index][channel_index];

	for (int i = 0; i < filter_size; i++)
	{
		double * ptr = weight.ptr<double>(i);
		for (int j = 0; j < filter_size; j++)
		{
			cout<<ptr[j]<<" ";
		}
		cout<<endl;
	}
	cout<<endl;
	return 0;
}

int Conv::print_biaes()
{
	cout<<endl<<name<<"  biases"<<endl;
	for (int i = 0; i < filters; i++)
	{
		cout<<biases[i]<<"  ";
	}
	cout<<endl<<endl;

	return 0;
}

Conv::Conv()
{
	weights = NULL;
	biases = NULL;
}

Conv::~Conv()
{
	//cout<<"conv delete"<<endl;

	if (weights!=NULL)
	{
		for (int i = 0; i < filters; i++)
		{
			delete[] weights[i];
			weights[i] = NULL;
		}

		delete[] weights;
		weights = NULL;
	}

	if (biases!=NULL)
	{
		delete[] biases;
		biases = NULL;
	}
	
}


int Conv::nonlinear_cvt(Mat * img, int n, string type)
{
	//神经元非线性变化，类型为none, relu,logistic
	if (type == "none")
	{
		return 0;
	}

	if (type == "relu")
	{
		for (int channel_index = 0 ; channel_index < n; channel_index++)
		{
			for (int m = 0; m < img[channel_index].rows; m++)
			{
				double * img_ptr = img[channel_index].ptr<double>(m);
				for (int n = 0; n < img[channel_index].cols; n++)
				{
					if (img_ptr[n] < 0)
					{
						img_ptr[n] = 0;
					}
				}
			}
		}
		return 0;
	}

	if (type == "logistic")
	{
		for (int channel_index = 0 ; channel_index < n; channel_index++)
		{
			for (int m = 0; m < img[channel_index].rows; m++)
			{
				double * img_ptr = img[channel_index].ptr<double>(m);
				for (int n = 0; n < img[channel_index].cols; n++)
				{
					img_ptr[n] = 1.0/(1.0+ exp(-1.0 * exp(img_ptr[n])));
				}
			}
		}
		return 0;
	}

	return ERROR_NEURON_INVALID;
}
