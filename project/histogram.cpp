using namespace std;
using namespace cv;

/*This function generates the LBP for the Data Matrix */
template <typename _Tp>
Mat LocalBinaryPattern(Mat src,int radius, int neighbors)
{
	Mat dst;
	neighbors = max(min(neighbors,31),1);
	dst = Mat::zeros(src.rows-2*radius, src.cols-2*radius, CV_32SC1);
	for(int n=0; n<neighbors; n++)
	{
		/* 1.Sample points */
		float x = static_cast<float>(radius) * cos(2.0*M_PI*n/static_cast<float>(neighbors));
		float y = static_cast<float>(radius) * -sin(2.0*M_PI*n/static_cast<float>(neighbors));
		/* 2. Relative indices */
		int fx = static_cast<int>(floor(x));
		int fy = static_cast<int>(floor(y));
		int cx = static_cast<int>(ceil(x));
		int cy = static_cast<int>(ceil(y));
		/* 3. Fractional part */
		float ty = y - fy;
		float tx = x - fx;
		/* 4. Set interpolation weights */
		float w1 = (1 - tx) * (1 - ty);
		float w2 =      tx  * (1 - ty);
		float w3 = (1 - tx) *      ty;
		float w4 =      tx  *      ty;
		/* 5. iterate through the data matrix */
		for(int i=radius; i < src.rows-radius;i++)
		{
			for(int j=radius;j < src.cols-radius;j++)
			{
				float t = w1*src.at<_Tp>(i+fy,j+fx) + w2*src.at<_Tp>(i+fy,j+cx) + w3*src.at<_Tp>(i+cy,j+fx) + w4*src.at<_Tp>(i+cy,j+cx);
				dst.at<unsigned int>(i-radius,j-radius) += ((t > src.at<_Tp>(i,j)) && (abs(t-src.at<_Tp>(i,j)) > std::numeric_limits<float>::epsilon())) << n;    /*add tolerance */
			}
		}
	}

	normalize(dst,dst, 0, 255, NORM_MINMAX, CV_8UC1);
	Mat vec = dst.reshape(1,1);
	return vec;
}

/* This function choose the type of tne data matrix and invokes the LBP */
Mat Histogram(Mat src,int radius, int neighbors)
{
	Mat vec;
	switch(src.type())
	{
		case CV_8SC1:
			vec = LocalBinaryPattern<char>(src, radius, neighbors);
		break;
		case CV_8UC1:
			vec = LocalBinaryPattern<unsigned char>(src, radius, neighbors);
		break;
		case CV_16SC1:
			vec = LocalBinaryPattern<short>(src,radius, neighbors);
		break;
		case CV_16UC1:
			vec = LocalBinaryPattern<unsigned short>(src,radius, neighbors);
		break;
		case CV_32SC1:
			vec = LocalBinaryPattern<int>(src, radius, neighbors);
		break;
		case CV_32FC1:
			vec = LocalBinaryPattern<float>(src, radius, neighbors);
		break;
		case CV_64FC1:
			vec = LocalBinaryPattern<double>(src, radius, neighbors);
		break;
	}
	return vec;
}