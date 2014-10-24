#include "Cmrnorm.h"

using namespace  std;
using namespace cv;

Cmrnorm::Cmrnorm()
{}

Cmrnorm::Cmrnorm(string type_input, int channels_input, int size_input, double scale_input, double pow_input)
{
	type = type_input;
	channels = channels_input;
	size = size_input;
	scale = scale_input;
	m_pow = pow_input;
}

int Cmrnorm::do_norm(Mat * input, Mat * output)
{
	int img_size = input[0].rows;

	Mat tmp_img(img_size, img_size, CV_64FC1, Scalar::all(0));

	// middle variable, calculate input square
	Mat * input_square = new Mat[channels];
	if (input_square == NULL) {return ERROR_MALLOC_MEMORY;}
	for (int i = 0; i < channels; i++)
	{
		tmp_img = input[i].mul(input[i]);
		tmp_img.copyTo(input_square[i]);
	}

	for (int channel_index = 0; channel_index < channels; channel_index++)
	{
		tmp_img.setTo(0);
		int start = (0 > (channel_index - size/2) ? 0:(channel_index - size/2));
		int end = ( (channels -1) < (channel_index + size/2) ? (channels -1) : (channel_index + size/2) );

		for (int i = start; i <= end; i++)
		{
			tmp_img += input_square[i];
		}

		tmp_img = tmp_img * scale / (double)(size) + 1.0;

		cv::pow(tmp_img, m_pow, tmp_img);
		cv::divide(input[channel_index],tmp_img, tmp_img);

		tmp_img.copyTo(output[channel_index]);
	}

	delete[] input_square;
}