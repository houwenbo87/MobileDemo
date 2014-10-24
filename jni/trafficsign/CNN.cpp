#include "CNN.h"
#include <math.h>
#include <fstream>

using namespace std;
using namespace cv;

int CNN::initialise(string model_path_input)
{
	int ret_code = 0;

	model_path = model_path_input;
	input_img_channels = INPUT_IMG_CHANNELS;
	input_img_size = INPUT_IMG_SIZE;

	ret_code = load_mean_data();

	// allocate input_data
	input_data = new Mat[input_img_channels];
	if (input_data==NULL) {	return ERROR_MALLOC_MEMORY; }
	for (int i = 0; i < input_img_channels; i++)
	{
		input_data[i].create(input_img_size, input_img_size, CV_64FC1);
	}

	/// conv1
	conv1 = Conv("conv1", 16,2,1,5,3,"relu");
	ret_code = conv1.initialise(model_path);
	conv1_output = new Mat[conv1.filters];
	if (conv1_output==NULL) {	return  ERROR_MALLOC_MEMORY;	}

	//pool1
	pool1 = Pool("max", 3, 2, conv1.filters);
	pool1_output = new Mat[pool1.channels];
	if (pool1_output == NULL) { return ERROR_MALLOC_MEMORY; }

	//rnorm1
	rnorm1 = Cmrnorm("cmrnorm", 16, 5, 0.0001,0.75);
	rnorm1_output = new Mat[rnorm1.channels];
	if (rnorm1_output == NULL) { return ERROR_MALLOC_MEMORY;}

	/// conv2
	conv2 = Conv("conv2", 32,2,1,5,16,"relu");
	ret_code = conv2.initialise(model_path);
	conv2_output = new Mat[conv2.filters];
	if (conv2_output==NULL) {	return  ERROR_MALLOC_MEMORY;	}


	//rnorm2
	rnorm2 = Cmrnorm("cmrnorm", 32, 5, 0.0001,0.75);
	rnorm2_output = new Mat[rnorm2.channels];
	if (rnorm2_output == NULL) { return ERROR_MALLOC_MEMORY;}

	//pool2
	pool2 = Pool("max", 3, 2, conv2.filters);
	pool2_output = new Mat[pool2.channels];
	if (pool2_output == NULL) { return ERROR_MALLOC_MEMORY; }

	/// conv3
	conv3 = Conv("conv3", 64,2,1,5,32,"relu");
	ret_code = conv3.initialise(model_path);
	conv3_output = new Mat[conv3.filters];
	if (conv2_output==NULL) {	return  ERROR_MALLOC_MEMORY;	}


	//rnorm3
	rnorm3 = Cmrnorm("cmrnorm", 64, 5, 0.0001,0.75);
	rnorm3_output = new Mat[rnorm3.channels];
	if (rnorm3_output == NULL) { return ERROR_MALLOC_MEMORY;}

	//pool3
	pool3 = Pool("max", 3, 2, conv3.filters);
	pool3_output = new Mat[pool3.channels];
	if (pool3_output == NULL) { return ERROR_MALLOC_MEMORY; }

	//fc1024_1
	int fc1024_1_input_dim = 2304; // if input_img_size is not 48*48*3, this number should be changed 
	fc1024_1 = FullConnect("fc1024_1", 512, "logistic", fc1024_1_input_dim);
	fc1024_1.initialise(model_path);

	//fc2
	//int fc1024_1_input_dim = 2304; // if input_img_size is not 48*48*3, this number should be changed 
	fc2 = FullConnect("fc2", 18, "none", fc1024_1.outputs);
	fc2.initialise(model_path);

	cout<<"INFO: cnn initialise ok"<<endl;

	return ret_code;
}

int CNN::do_cnn(Mat img, vector<double> & result)
{
	if (check_input(img) != 0)
	{
		return ERROR_INPUT_INVALID;
	}
	int ret_code = 0;

	/*
	double t = (double)getTickCount();
	generate_input_data(img);
	t = (double)getTickCount() - t;
	//cout<<"gene "<<t/getTickFrequency()<<endl;
	
	//conv1
	t = (double)getTickCount();
	ret_code = conv1.do_conv(input_data, conv1_output);
	t = (double)getTickCount() - t;
	//cout<<"conv "<<t/getTickFrequency()<<endl;


	t = (double)getTickCount();
	// pool1
	ret_code = pool1.do_pool(conv1_output, pool1_output);
	t = (double)getTickCount() - t;
	//cout<<"pool "<<t/getTickFrequency()<<endl;

	t = (double)getTickCount();
	// norm1
	ret_code = rnorm1.do_norm(pool1_output, rnorm1_output);
	t = (double)getTickCount() - t;
	//cout<<"norm "<<t/getTickFrequency()<<endl;


	t = (double)getTickCount();
	//conv2
	ret_code = conv2.do_conv(rnorm1_output, conv2_output);
	t = (double)getTickCount() - t;
	//cout<<"conv2 "<<t/getTickFrequency()<<endl;

	t = (double)getTickCount();
	// norm2
	ret_code = rnorm2.do_norm(conv2_output, rnorm2_output);
	t = (double)getTickCount() - t;
	//cout<<"norm2  "<<t/getTickFrequency()<<endl;

	t = (double)getTickCount();
	// pool2
	ret_code = pool2.do_pool(rnorm2_output, pool2_output);
	t = (double)getTickCount() - t;
	//cout<<"pool2  "<<t/getTickFrequency()<<endl;

	//conv3
	t = (double)getTickCount();
	ret_code = conv3.do_conv(pool2_output, conv3_output);
	t = (double)getTickCount() - t;
	//cout<<"conv3  "<<t/getTickFrequency()<<endl;

	// norm3
	t = (double)getTickCount();
	ret_code = rnorm3.do_norm(conv3_output, rnorm3_output);
	t = (double)getTickCount() - t;
	//cout<<"norm3  "<<t/getTickFrequency()<<endl;

	// pool3
	t = (double)getTickCount();
	ret_code = pool3.do_pool(rnorm3_output, pool3_output);
	t = (double)getTickCount() - t;
	//cout<<"pool3  "<<t/getTickFrequency()<<endl;

	//fc1024_1
	//cvt 3d data of poo3_ouput to 1d vector
	t = (double)getTickCount();
	Mat pool3_ouput_vec;
	cvt_mat_array_to_vector(pool3_output, pool3.channels, pool3_ouput_vec);
	//write_to_file(&pool3_ouput_vec, 1);
	//cout<<"ok1"<<endl<<pool3_ouput_vec.rows<<endl;
	ret_code = fc1024_1.do_full_connect(pool3_ouput_vec, fc1024_1_output);
	t = (double)getTickCount() - t;
	//cout<<"fc1  "<<t/getTickFrequency()<<endl;

	//fc_2
	t = (double)getTickCount();
	ret_code = fc2.do_full_connect(fc1024_1_output,fc2_output);
	t = (double)getTickCount() - t;
	//cout<<"fc2  "<<t/getTickFrequency()<<endl;

	//soft_max
	ret_code = soft_max();
	*/


	generate_input_data(img);

	
	//conv1
	ret_code = conv1.do_conv(input_data, conv1_output);

	// pool1
	ret_code = pool1.do_pool(conv1_output, pool1_output);

	// norm1
	ret_code = rnorm1.do_norm(pool1_output, rnorm1_output);

	//conv2
	ret_code = conv2.do_conv(rnorm1_output, conv2_output);

	// norm2
	ret_code = rnorm2.do_norm(conv2_output, rnorm2_output);

	// pool2
	ret_code = pool2.do_pool(rnorm2_output, pool2_output);

	//conv3
	ret_code = conv3.do_conv(pool2_output, conv3_output);

	// norm3
	ret_code = rnorm3.do_norm(conv3_output, rnorm3_output);

	// pool3
	ret_code = pool3.do_pool(rnorm3_output, pool3_output);

	//fc1024_1
	//cvt 3d data of poo3_ouput to 1d vector
	Mat pool3_ouput_vec;
	cvt_mat_array_to_vector(pool3_output, pool3.channels, pool3_ouput_vec);
	ret_code = fc1024_1.do_full_connect(pool3_ouput_vec, fc1024_1_output);

	//fc_2
	ret_code = fc2.do_full_connect(fc1024_1_output,fc2_output);

	//soft_max
	ret_code = soft_max(result);

	return ret_code;
}

int CNN::soft_max(vector<double> & result)
{
	result.clear();
	double tmp_sum = 0;
	double * fc2_output_ptr = NULL;
	for (int i = 0; i < fc2_output.rows; i++)
	{
		fc2_output_ptr = fc2_output.ptr<double>(i);
		fc2_output_ptr[0] = exp(fc2_output_ptr[0]);
		tmp_sum += fc2_output_ptr[0];
	}

	for (int i = 0; i < fc2_output.rows; i++)
	{
		fc2_output_ptr = fc2_output.ptr<double>(i);
		fc2_output_ptr[0] /= tmp_sum;
		result.push_back(fc2_output_ptr[0]);
	}
	return 0;
}

int CNN::cvt_mat_array_to_vector(Mat * mat_array, int array_length, Mat & output)
{
	int output_dims = mat_array[0].rows * mat_array[0].cols * array_length;
	output.create(output_dims, 1, CV_64FC1);
	output.setTo(0);

	int num = 0;
	double * output_ptr = NULL;
	for (int channel_index = 0; channel_index < array_length; channel_index++)
	{
		for (int m = 0; m < mat_array[channel_index].rows; m++)
		{
			double * mat_array_ptr = mat_array[channel_index].ptr<double>(m);
			for (int n = 0; n < mat_array[channel_index].cols; n++)
			{
				output_ptr = output.ptr<double>(num);
				output_ptr[0] = mat_array_ptr[n];
				num++;
			}
		}
	}
	return 0;
}

int CNN::generate_input_data(Mat img)
{
	for (int channel_index = 0; channel_index < input_img_channels; channel_index++)
	{
		for (int m = 0; m < input_img_size; m++)
		{
			double * input_data_ptr = input_data[channel_index].ptr<double>(m);
			double * mean_data_ptr = mean_data[channel_index].ptr<double>(m);
			unsigned char * img_ptr = img.ptr<unsigned char>(m);
			 
			for (int n = 0; n < input_img_size; n++)
			{
				input_data_ptr[n] = (double)(img_ptr[n*input_img_channels + channel_index]) - mean_data_ptr[n];
			}
		}
	}

	return 0;
}

int CNN::test(Mat img)
{
	vector<double> result;
	do_cnn(img, result);

	//write_to_file(pool3_output, pool3.channels);
	//write_to_file(&fc2_output, 1);
	
	return 0;
}

int CNN::load_mean_data()
{
	// load mean data
	ifstream input_file((model_path + "data_mean.txt").c_str());
	if (input_file.fail())
	{
		cout<<"ERROR: load model file "<<model_path + "data_mean.txt"<<endl;
		return ERROR_INITIAL_CNN_MODEL;
	}

	mean_data = new Mat[input_img_channels];
	if (mean_data == NULL)  {return ERROR_MALLOC_MEMORY;}
	for (int i = 0; i < input_img_channels; i++)
	{
		mean_data[i].create(input_img_size, input_img_size, CV_64FC1);
	}
	for (int channel_index = 0; channel_index < input_img_channels; channel_index++)
	{
		for (int m = 0; m < input_img_size; m++)
		{
			double * ptr = mean_data[channel_index].ptr<double>(m);
			for (int n = 0; n < input_img_size; n++)
			{
				input_file>>ptr[n];
			}
		}
	}

	input_file.close();

	return 0;
}


int CNN::check_input(Mat input_img)
{
	int ret_code = 0;
	if (input_img.rows != input_img_size || input_img.cols!=input_img_size || input_img.channels()!=input_img_channels)
	{
		return ERROR_INPUT_INVALID;
	}
	return 0;
}

int CNN::write_to_file(Mat * mat_array, int n)
{
	ofstream output("test.txt");

	for (int i = 0; i < n; i++)
	{
		for (int m = 0; m < mat_array[i].rows; m++)
		{
			double * ptr = mat_array[i].ptr<double>(m);
			for (int n = 0; n < mat_array[i].cols; n++)
			{
				output<<ptr[n]<<endl;
			}
		}
	}

	output.close();
	return 0;
}

CNN::CNN()
{
	//conv1 = Conv("conv1", 16,2,1,5,3,"relu");
	//conv1 = NULL;

	mean_data = NULL;
	input_data = NULL;
	conv1_output = NULL;
	pool1_output = NULL;
}

CNN::~CNN()
{
	//cout<<"aaaaa"<<endl;
	if (mean_data != NULL)
	{
		delete[] mean_data;
		mean_data = NULL;
	}

	if (conv1_output != NULL)
	{
		delete[] conv1_output;
		conv1_output  = NULL;
	}

	if (input_data !=NULL)
	{
		delete[] input_data;
		input_data = NULL;
	}

	if (pool1_output != NULL)
	{
		delete[] pool1_output;
		pool1_output = NULL;
	}

	
	if (rnorm1_output != NULL)
	{
		delete[] rnorm1_output;
		rnorm1_output = NULL;
	}


	if (conv2_output != NULL)
	{
		delete[] conv2_output;
		conv2_output = NULL;
	}

	if (rnorm2_output != NULL)
	{
		delete[] rnorm2_output;
		rnorm2_output = NULL;
	}	


	if (pool2_output != NULL)
	{
		delete[] pool2_output;
		pool2_output = NULL;
	}

	if (conv3_output != NULL)
	{
		delete[] conv3_output;
		conv3_output = NULL;
	}

	if (rnorm3_output != NULL)
	{
		delete[] rnorm3_output;
		rnorm3_output = NULL;
	}	

	if (pool3_output != NULL)
	{
		delete[] pool3_output;
		pool3_output = NULL;
	}
}