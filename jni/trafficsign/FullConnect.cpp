#include "FullConnect.h"
#include <fstream>
#include <math.h>

using namespace std;
using namespace cv;

FullConnect::FullConnect()
{
	name = "";
	neuron = "";
	biases = NULL;
}

FullConnect::FullConnect(string name_input, int outputs_input, string neuron_input, int input_dim_input)
{
	name = name_input;
	outputs = outputs_input;
	neuron = neuron_input;
	input_dim = input_dim_input;

	biases = NULL;
}

int FullConnect::initialise(string model_savepath)
{
	/// load weights
	string weights_filename = model_savepath + name + "_weights.txt";
	ifstream weights_file(weights_filename.c_str());
	if (weights_file.fail()) {return ERROR_INITIAL_CNN_MODEL;}

	weights.create(input_dim, outputs, CV_64FC1);
	for (int i = 0; i < input_dim; i++)
	{
		double * weights_ptr = weights.ptr<double>(i);
		for (int j = 0; j < outputs; j++)
		{
			weights_file>>weights_ptr[j];
		}
	}
	weights_file.close();

	weights = weights.t();

	string biases_filename = model_savepath + name + "_biases.txt";
	ifstream biases_file(biases_filename.c_str());
	if (biases_file.fail()) {return ERROR_INITIAL_CNN_MODEL;}

	biases = new double[outputs];
	if (biases == NULL) {return ERROR_MALLOC_MEMORY;}

	for (int i = 0; i < outputs; i++)
	{
		biases_file>>biases[i];
	}
	biases_file.close();

	return 0;
}

int FullConnect::do_full_connect(Mat  input, Mat & output_data)
{
	
	double t = (double)getTickCount();
	output_data = weights * input;
	t = (getTickCount()-t)/getTickFrequency();
	//cout<<"matrix mul: "<<t<<endl;

	

	double * output_ptr;
	for (int i = 0; i < output_data.rows; i++)
	{
		output_ptr = output_data.ptr<double>(i);
		output_ptr[0] += biases[i];
	}

	
	if (neuron == "logistic")
	{
		for (int i = 0; i < output_data.rows; i++)
		{
			output_ptr = output_data.ptr<double>(i);
			output_ptr[0] = 1.0/(1.0+exp(-output_ptr[0]));
		}
	}
	

	return 0;   
}


FullConnect::~FullConnect()
{
	if (biases!=NULL)
	{
		delete[] biases;
		biases = NULL;
	}
}