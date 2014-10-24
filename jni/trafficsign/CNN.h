#include "Conv.h"
#include "common.h"
#include "Pool.h"
#include "Cmrnorm.h"
#include "FullConnect.h"

#define INPUT_IMG_CHANNELS	3
#define INPUT_IMG_SIZE		48

class CNN
{
public:
	Mat * mean_data;

	int input_img_channels, input_img_size;
	string model_path;

	int initialise(string model_path_input);
	int test(Mat img);

	int do_cnn(Mat img, vector<double> & result);

	CNN();
	~CNN();
private:
	Mat * input_data;

	//conv1
	Conv  conv1;
	Mat * conv1_output;

	//pool1
	Pool pool1;
	Mat * pool1_output;

	// rnorm1
	Cmrnorm rnorm1;
	Mat * rnorm1_output;

	//conv2
	Conv conv2;
	Mat * conv2_output;

	// rnorm2
	Cmrnorm rnorm2;
	Mat * rnorm2_output;

	//pool2
	Pool pool2;
	Mat * pool2_output;

	//conv2
	Conv conv3;
	Mat * conv3_output;

	// rnorm2
	Cmrnorm rnorm3;
	Mat * rnorm3_output;

	//pool3
	Pool pool3;
	Mat * pool3_output;

	//fc_1
	FullConnect fc1024_1;
	Mat fc1024_1_output;

	//fc_2
	FullConnect fc2;
	Mat fc2_output;

	//vector<double> result;


	int soft_max(vector<double> & result);

	int load_mean_data();
	int check_input(Mat input_img);

	int generate_input_data(Mat img);

	int cvt_mat_array_to_vector(Mat * mat_array, int array_length, Mat & output);

	//for test
	int write_to_file(Mat * mat_array, int n);
};