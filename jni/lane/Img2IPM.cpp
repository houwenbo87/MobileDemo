#include "Img2IPM.h"

int Img2IPM::process(Point pt[4], string save_path, int img_cols, int img_rows)
{
	int pointNum = 4;
	Point2f * srcPoint = new Point2f[pointNum];
	Point2f * dstPoint = new Point2f[pointNum];

	CvSize ipmSize;


	//v2 0514
	ipmSize.width = 260;
	ipmSize.height = 300;
	
	//sample4  °üº¬³µÍ·
	{
		/// sample1 and sample3
		srcPoint[0] = cvPoint(pt[0].x, pt[0].y);
		srcPoint[1] = cvPoint(pt[1].x, pt[1].y);
		srcPoint[2] = cvPoint(pt[2].x, pt[2].y);
		srcPoint[3] = cvPoint(pt[3].x, pt[3].y);

		//v2 0514
		dstPoint[0] = cvPoint(105, 284);
		dstPoint[1] = cvPoint(154, 284);
		dstPoint[2] = cvPoint(154, 11);
		dstPoint[3] = cvPoint(105, 11);
	}

	Mat img2ipmMatrix = getPerspectiveTransform(srcPoint, dstPoint);
	Mat ipm2imgMatrix = getPerspectiveTransform(dstPoint, srcPoint);

	ofstream ipm2iniFile((save_path + "ipm2ini.txt").c_str());
	for (int i = 0; i < ipm2imgMatrix.rows; i++)
	{
		double * aa = ipm2imgMatrix.ptr<double>(i);
		for (int j = 0; j < ipm2imgMatrix.cols; j++)
		{
			ipm2iniFile<<aa[j]<<" ";
		}
		ipm2iniFile<<endl;
	}
	ipm2iniFile.close();

	ofstream ini2ipmFile((save_path + "ini2ipm.txt").c_str());
	for (int i = 0; i < img2ipmMatrix.rows; i++)
	{
		double * aa = img2ipmMatrix.ptr<double>(i);
		for (int j = 0; j < img2ipmMatrix.cols; j++)
		{
			ini2ipmFile<<aa[j]<<" ";
		}
		ini2ipmFile<<endl;
	}
	ini2ipmFile.close();

	

	ofstream outlierFile((save_path +"outlier.txt").c_str());
	vector<Point2f> allIpmPoints;
	vector<Point2f> allIpmPoints2IniImg;
	int num2 = 0;
	for (int m = 0; m < ipmSize.height; m++)
	{
		for (int n = 0; n < ipmSize.width; n++)
		{
			allIpmPoints.push_back(cvPoint(n,m));
			num2++;
		}
	}
	perspectiveTransform(allIpmPoints, allIpmPoints2IniImg, ipm2imgMatrix);
	num2 = 0;
	for (int m = 0; m < ipmSize.height; m++)
	{
		for (int n = 0; n < ipmSize.width; n++)
		{
			Point2f tmpPt = allIpmPoints2IniImg[num2];
			if (tmpPt.x < 5 || tmpPt.x > img_cols - 5 || tmpPt.y < 5 || tmpPt.y > img_rows - 5)
			{
				// outlier
				outlierFile<<n<<" "<<m<<endl;
			}
			num2++;
		}
	}
	outlierFile.close();

	delete [] srcPoint;
	delete [] dstPoint;

	return 0;
}
