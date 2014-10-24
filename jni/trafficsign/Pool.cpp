#include "Pool.h"

using namespace std;
using namespace cv;

Pool::Pool(string pool_input, int sizeX_input, int stride_input, int channels_input)
{
	pool = pool_input;
	sizeX = sizeX_input;
	stride = stride_input;
	channels = channels_input;
}

int Pool::do_pool(Mat * img, Mat * output)
{
	// pool type: max, avg

	int img_size = img[0].rows;
	if (pool == "max")
	{
		Mat tmp_out(img_size/stride, img_size/stride, CV_64FC1, Scalar::all(0));
		double min = 0, max = 0,temp_double = 0;

		for (int channel_index = 0; channel_index < channels; channel_index++)
		{
			int out_row = 0, out_col = 0;
			for (int m = 0; m < img_size ; m += stride)
			{
				double * out_ptr = tmp_out.ptr<double>(out_row);
				out_col = 0;
				for (int n = 0; n < img_size ; n += stride)
				{
					int width = (sizeX <img_size - n) ? sizeX :(img_size - n);
					int height = (sizeX <img_size - m) ? sizeX :(img_size - m );

					//minMaxIdx(img[channel_index](Rect(n,m,width, height)), &min, &max);
					max = img[channel_index].at<double>(m,n);
					for (int row_index = m; row_index < m +height; row_index++)
					{
						for (int col_index = n; col_index < n + width; col_index++)
						{
							temp_double = img[channel_index].at<double>(row_index,col_index);
							if (temp_double > max)
							{
								max = temp_double;
							}
						}
					}

					out_ptr[out_col] = max;
					out_col++;
				}
				out_row++;
			}
			//output[channel_index].release();
			tmp_out.copyTo(output[channel_index]);
		}

		return 0;
	}
	return ERROR_POOL_INVALID;
}

Pool::Pool()
{
	//cout<<"pool construct"<<endl;
}