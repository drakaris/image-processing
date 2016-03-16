
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <limits>
#include <unistd.h>

using namespace std;
using namespace cv;

// Function Headers
void detectAndDisplay(Mat img);
void cropFaces(Mat img , std::vector<Rect> faces );
void FilterFaces(Mat img ,Mat res);
void histogram(Mat src,int radius, int neighbors);
template <typename _Tp>
void ELBP_(Mat src,int radius, int neighbors);
void ClusterFaces();

// Global variables
string faceCascadeFilename; 
CascadeClassifier faceDetector;
Mat image_array;
vector<Mat> source_array;

string window_name = "Capture - Face detection";
int filenumber = 100; // Number of file to be saved

// Function main
int main(void)
{
    int choice = 1;
    //cout << "Choose Detector Type : \n 1.Haar Cascade Default \n 2. Fast Haar Cascade \n 3.LBP Detector \n 4.Profile Detector \n";
    //cin >> choice ;

    
    //choose cascade
    switch(choice)
    {
        case 1:
            faceCascadeFilename = "lib/haarcascade_frontalface_default.xml";
            break;
        case 2:
            faceCascadeFilename = "lib/haarcascade_frontalface_alt2.xml";
            break;
        case 3:
            faceCascadeFilename = "lib/lbpcascade_frontalface.xml";
            break;
        case 4:
            faceCascadeFilename = "lib/haarcascade_profileface.xml";
            break;
        default:
            faceCascadeFilename = "lib/haarcascade_frontalface_default";
            break;
    }
    
    // Load the cascade
    try
    {
        faceDetector.load(faceCascadeFilename);
    }
    catch (cv::Exception e) {}
    if ( faceDetector.empty() )
    {
        cout << "ERROR: Couldn't load Face Detector (";
        cout << faceCascadeFilename << ")!" << endl;
        exit(1);
    }
    
    // Read the image file
    string image_file_name;
    Mat img;
    cout << "Enter File Name ----    ";
    cin >> image_file_name;

    std::ifstream file(image_file_name.c_str(), ifstream::in);
    if (!file)
    {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(Error::StsBadArg, error_message);
    }
    string line, path, classlabel;
    while (getline(file, line))
    {
        stringstream liness(line);
        img = imread(line);
        // Apply the classifier to the fram
        if (!img.empty())
        {
            detectAndDisplay(img);
        }
        else
        {
            cout << " Image Not Found";
            exit(1);
        }
    }
    cout << "Size of the Image Array " << image_array.size() << "\n";
    ClusterFaces();
    //waitKey(0);
    return 0;
}

// Function detectAndDisplay

void detectAndDisplay(Mat img)
{

    //CONVERT TO GRAY SCALE
    Mat gray;
    if (img.channels() == 3)
        cvtColor(img, gray, CV_BGR2GRAY);
    else if (img.channels() == 4)
        cvtColor(img, gray, CV_BGRA2GRAY);
    else
        gray = img;
    //imshow("Gray Scale",gray);

    const int scaledWidth = 320;
    // Possibly shrink the image, to run much faster.
    Mat smallImg;
    float scale = img.cols / (float) scaledWidth;
    if (img.cols > scaledWidth)
    {
        // Shrink the image while keeping the same aspect ratio.
        int scaledHeight = cvRound(img.rows / scale);
        resize(gray, smallImg, Size(scaledWidth, scaledHeight));
    }
    else
    {
        // Access the input directly since it is already small.
        smallImg = gray;
    }
    //imshow("Gray Scale",smallImg);

    // Standardize the brightness & contrast,to improve dark images.
    Mat equalizedImg;
    //equalizeHist(smallImg,equalizedImg);
    equalizeHist(gray,equalizedImg);
    //imshow("Hist",equalizedImg);

    int flags = CASCADE_SCALE_IMAGE; // Search for many faces.
    Size minFeatureSize(20, 20); // Smallest face size.
    float searchScaleFactor = 1.1f; // How many sizes to search.
    int minNeighbors = 4; // Reliability vs many faces.
    // Detect faces in the small grayscale image.
    std::vector<Rect> faces;
    faceDetector.detectMultiScale(equalizedImg, faces, searchScaleFactor,minNeighbors,flags,minFeatureSize);
    cropFaces(img,faces);

    /*

    // Enlarge the results if the image was temporarily shrunk.
    if (img.cols > scaledWidth)
    {
        for (int i = 0; i < (int)objects.size(); i++ )
        {
            objects[i].x = cvRound(objects[i].x * scale);
            objects[i].y = cvRound(objects[i].y * scale);
            objects[i].width = cvRound(objects[i].width * scale);
            objects[i].height = cvRound(objects[i].height * scale);
        }
    }
    // If the object is on a border, keep it in the image.
    for (int i = 0; i < (int)objects.size(); i++ )
    {
        if (objects[i].x < 0)
            objects[i].x = 0;
        if (objects[i].y < 0)
            objects[i].y = 0;
        if (objects[i].x + objects[i].width > img.cols)
            objects[i].x = img.cols - objects[i].width;
        if (objects[i].y + objects[i].height > img.rows)
            objects[i].y = img.rows - objects[i].height;
    }
    */


}



void cropFaces(Mat img , std::vector<Rect> faces)
{
    // Set Region of Interest
    string text;
    Mat crop,res;
    Mat display_image;
    cvtColor(img, display_image, CV_RGB2GRAY);
    cv::Rect roi_b;
    cv::Rect roi_c;

    size_t ic = 0; // ic is index of current element
    int ac = 0; // ac is area of current element

    size_t ib = 0; // ib is index of biggest element
    int ab = 0; // ab is area of biggest element

    for (ic = 0; ic < faces.size(); ic++) // Iterate through all current elements (detected faces)

    {
        roi_c.x = faces[ic].x;
        roi_c.y = faces[ic].y;
        roi_c.width = (faces[ic].width);
        roi_c.height = (faces[ic].height);

        ac = roi_c.width * roi_c.height; // Get the area of current element (detected face)

        roi_b.x = faces[ib].x;
        roi_b.y = faces[ib].y;
        roi_b.width = (faces[ib].width);
        roi_b.height = (faces[ib].height);

        ab = roi_b.width * roi_b.height; // Get the area of biggest element, at beginning it is same as "current" element

        if (ac > ab)
        {
            ib = ic;
            roi_b.x = faces[ib].x;
            roi_b.y = faces[ib].y;
            roi_b.width = (faces[ib].width);
            roi_b.height = (faces[ib].height);
        }

        Point pt1(faces[ic].x, faces[ic].y);
        Point pt2((faces[ic].x + faces[ic].height), (faces[ic].y + faces[ic].width));
        rectangle(display_image, pt1, pt2, Scalar(0, 255, 0), 2, 8, 0); //draw detected faces in image

        crop = img(Rect(pt1,pt2));
        resize(crop, res, Size(128, 128), 0, 0, INTER_LINEAR); // Uniformly resize all images for preprocessing
        FilterFaces(img,res);
    }

    putText(display_image, text, cvPoint(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
    namedWindow( "original", WINDOW_NORMAL);
    //resizeWindow("original", 300, 300);
    //imshow("original", display_image);
}




void FilterFaces(Mat img , Mat frame)
{

    CascadeClassifier mcs_lefteye,mcs_righteye,left_eye_split,right_eye_split,default_eye,tree_eyeglasses;

    if( !mcs_lefteye.load("lib/haarcascade_mcs_lefteye.xml") ){ printf("--(!)Error loading Left eyes cascade\n"); exit(0); };
    if( !mcs_righteye.load("lib/haarcascade_mcs_righteye.xml") ){ printf("--(!)Error loading Right eyes cascade\n"); exit(0); };
    if( !left_eye_split.load( "lib/haarcascade_lefteye_2splits.xml" ) ){ printf("--(!)Error loading Left eyes split cascade\n"); exit(0); };
    if( !right_eye_split.load( "lib/haarcascade_righteye_2splits.xml" ) ){ printf("--(!)Error loading Right eyes split cascade\n"); exit(0); };
    if( !default_eye.load( "lib/haarcascade_eye.xml" ) ){ printf("--(!)Error loading eyes cascade\n"); exit(0); };
    if( !tree_eyeglasses.load( "lib/haarcascade_eye_tree_eyeglasses.xml" ) ){ printf("--(!)Error loading Glass eyes cascade\n"); exit(0); };


    std::vector<Rect> eyes;
    std::vector<Rect> eyes_right;
    Mat frame_gray;
    int face_flag = 0;

    if (frame.channels() == 3)
        cvtColor(frame, frame_gray, CV_BGR2GRAY);
    else if (frame.channels() == 4)
        cvtColor(frame, frame_gray, CV_BGRA2GRAY);
    else
        frame_gray = frame;

    equalizeHist( frame_gray, frame_gray );

    //-- In each face, detect eyes
    left_eye_split.detectMultiScale( frame_gray, eyes, 1.1, 2, 0 |CASCADE_FIND_BIGGEST_OBJECT | CASCADE_DO_ROUGH_SEARCH, Size(30, 30));
    if(eyes.size() > 0)
    {
        right_eye_split.detectMultiScale( frame_gray, eyes_right, 1.1, 2, 0 |CASCADE_FIND_BIGGEST_OBJECT | CASCADE_DO_ROUGH_SEARCH, Size(30, 30));
        //cout << "Size Of Left " << eyes.size() << "\t" << "Size of Right " << eyes_right.size() << "\n";
        if(eyes_right.size() > 0)
        {
            for( size_t j = 0; j < eyes_right.size(); j++ )
            {
                Point eye_center( eyes_right[j].x + eyes_right[j].width/2, eyes_right[j].y + eyes_right[j].height/2 );
                int radius = cvRound( (eyes_right[j].width + eyes_right[j].height)*0.25 );
                circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
            }
            for( size_t j = 0; j < eyes.size(); j++ )
            {
                Point eye_center( eyes[j].x + eyes[j].width/2, eyes[j].y + eyes[j].height/2 );
                int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
                circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
            }
            face_flag = 1;
        }
        else
        {
            mcs_righteye.detectMultiScale( frame_gray, eyes_right, 1.1, 2, 0 |CASCADE_FIND_BIGGEST_OBJECT | CASCADE_DO_ROUGH_SEARCH, Size(30, 30));
            //cout << "Size Of Left " << eyes.size() << "\t" << "Size of Right " << eyes_right.size() << "\n";
            if(eyes_right.size() > 0)
            {
                for( size_t j = 0; j < eyes_right.size(); j++ )
                {
                    Point eye_center( eyes_right[j].x + eyes_right[j].width/2, eyes_right[j].y + eyes_right[j].height/2 );
                    int radius = cvRound( (eyes_right[j].width + eyes_right[j].height)*0.25 );
                    circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
                }
                for( size_t j = 0; j < eyes.size(); j++ )
                {
                    Point eye_center( eyes[j].x + eyes[j].width/2, eyes[j].y + eyes[j].height/2 );
                    int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
                    circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
                }
                face_flag = 1;
            }
        }
    }
    if(face_flag ==0)
    {
        mcs_lefteye.detectMultiScale( frame_gray, eyes, 1.1, 2, 0 |CASCADE_FIND_BIGGEST_OBJECT | CASCADE_DO_ROUGH_SEARCH, Size(30, 30));
        if(eyes.size() > 0)
        {
            mcs_righteye.detectMultiScale( frame_gray, eyes_right, 1.1, 2, 0 |CASCADE_FIND_BIGGEST_OBJECT | CASCADE_DO_ROUGH_SEARCH, Size(30, 30));
            //cout << "Size Of Left " << eyes.size() << "\t" << "Size of Right " << eyes_right.size() << "\n";
            if(eyes_right.size() > 0)
            {
                for( size_t j = 0; j < eyes_right.size(); j++ )
                {
                    Point eye_center( eyes_right[j].x + eyes_right[j].width/2, eyes_right[j].y + eyes_right[j].height/2 );
                    int radius = cvRound( (eyes_right[j].width + eyes_right[j].height)*0.25 );
                    circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
                }
                for( size_t j = 0; j < eyes.size(); j++ )
                {
                    Point eye_center( eyes[j].x + eyes[j].width/2, eyes[j].y + eyes[j].height/2 );
                    int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
                    circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
                }
                face_flag = 1;
            }
            else
            {
                right_eye_split.detectMultiScale( frame_gray, eyes_right, 1.1, 2, 0 |CASCADE_FIND_BIGGEST_OBJECT | CASCADE_DO_ROUGH_SEARCH, Size(30, 30));
                //cout << "Size Of Left " << eyes.size() << "\t" << "Size of Right " << eyes_right.size() << "\n";
                if(eyes_right.size() > 0)
                {
                    for( size_t j = 0; j < eyes_right.size(); j++ )
                    {
                        Point eye_center( eyes_right[j].x + eyes_right[j].width/2, eyes_right[j].y + eyes_right[j].height/2 );
                        int radius = cvRound( (eyes_right[j].width + eyes_right[j].height)*0.25 );
                        circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
                    }
                    for( size_t j = 0; j < eyes.size(); j++ )
                    {
                        Point eye_center( eyes[j].x + eyes[j].width/2, eyes[j].y + eyes[j].height/2 );
                        int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
                        circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
                    }
                    face_flag = 1;
                }
            }
        }
    }
    if(face_flag == 0)
    {
        default_eye.detectMultiScale( frame_gray, eyes, 1.1, 2, 0 |CASCADE_FIND_BIGGEST_OBJECT , Size(30, 30));
        if(eyes.size() >= 1)
        {
            for( size_t j = 0; j < eyes.size(); j++ )
            {
                Point eye_center( eyes[j].x + eyes[j].width/2, eyes[j].y + eyes[j].height/2 );
                int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
                circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
            }
            face_flag = 1;
        }
    }

    if(face_flag == 0)
    {
        tree_eyeglasses.detectMultiScale( frame_gray, eyes, 1.1, 2, 0 |CASCADE_FIND_BIGGEST_OBJECT , Size(30, 30));
        if(eyes.size() >= 1)
        {
            for( size_t j = 0; j < eyes.size(); j++ )
            {
                Point eye_center( eyes[j].x + eyes[j].width/2, eyes[j].y + eyes[j].height/2 );
                int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
                circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
            }
            face_flag = 1;
        }

    }

    if(face_flag == 1)
    {
        //  Write the cropped image to a file
        source_array.push_back(img);
        Mat bilateral_img;
        bilateralFilter(frame_gray,bilateral_img,0, 20.0, 2.0,BORDER_DEFAULT );
        //imshow("Bit",bilateral_img);
        //imwrite(filename, bilateral_img);  //preprocessed_image
        histogram(bilateral_img,1,8);
    }
}




void histogram(Mat src,int radius, int neighbors)
{
	switch(src.type())
	{
		case CV_8SC1: ELBP_<char>(src, radius, neighbors); break;
		case CV_8UC1: ELBP_<unsigned char>(src, radius, neighbors); break;
		case CV_16SC1: ELBP_<short>(src,radius, neighbors); break;
		case CV_16UC1: ELBP_<unsigned short>(src,radius, neighbors); break;
		case CV_32SC1: ELBP_<int>(src, radius, neighbors); break;
		case CV_32FC1: ELBP_<float>(src, radius, neighbors); break;
		case CV_64FC1: ELBP_<double>(src, radius, neighbors); break;
	}
}




template <typename _Tp>
void ELBP_(Mat src,int radius, int neighbors)
{

	Mat dst;
	string filename;
	neighbors = max(min(neighbors,31),1);
	dst = Mat::zeros(src.rows-2*radius, src.cols-2*radius, CV_32SC1);
	for(int n=0; n<neighbors; n++)
	{
		// sample points
		float x = static_cast<float>(radius) * cos(2.0*M_PI*n/static_cast<float>(neighbors));
		float y = static_cast<float>(radius) * -sin(2.0*M_PI*n/static_cast<float>(neighbors));
		// relative indices
		int fx = static_cast<int>(floor(x));
		int fy = static_cast<int>(floor(y));
		int cx = static_cast<int>(ceil(x));
		int cy = static_cast<int>(ceil(y));
		// fractional part
		float ty = y - fy;
		float tx = x - fx;
		// set interpolation weights
		float w1 = (1 - tx) * (1 - ty);
		float w2 =      tx  * (1 - ty);
		float w3 = (1 - tx) *      ty;
		float w4 =      tx  *      ty;
		// iterate through your data
		for(int i=radius; i < src.rows-radius;i++)
		{
			for(int j=radius;j < src.cols-radius;j++)
			{
				float t = w1*src.at<_Tp>(i+fy,j+fx) + w2*src.at<_Tp>(i+fy,j+cx) + w3*src.at<_Tp>(i+cy,j+fx) + w4*src.at<_Tp>(i+cy,j+cx);
				// we are dealing with floating point precision, so add some little tolerance
				dst.at<unsigned int>(i-radius,j-radius) += ((t > src.at<_Tp>(i,j)) && (abs(t-src.at<_Tp>(i,j)) > std::numeric_limits<float>::epsilon())) << n;
			}
		}
	}

	filename = "";
	stringstream ssfn;
	ssfn << filenumber << ".png";
		filename = ssfn.str();
	filenumber++;
	normalize(dst,dst, 0, 255, NORM_MINMAX, CV_8UC1);
	//imwrite(filename, dst);
	//-- Show what you got
	namedWindow(window_name, WINDOW_NORMAL);
	//imshow( window_name, dst );
	Mat vec;
	vec = dst.reshape(1,1);
	image_array.push_back(vec);

}


void ClusterFaces()
{
    Mat points;
    image_array.convertTo(points, CV_32F);
    int clusterCount = 40;
    int i = 40;
    Mat centers;
    Mat labels;
    //for(int i = 40 ; i < clusterCount ; i++)
    //{
        kmeans(points, i , labels,TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0),3, KMEANS_PP_CENTERS, centers);

    //}
    
    //cout<<"Center: \n"<<centers<<endl;
    //cout<<"Labels: \n"<<labels<<endl;

    for(int i = 0 ; i < clusterCount ; i++)
    {
        stringstream ss;
        ss << i;
        string str = ss.str();
        mkdir(str.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }

    for(int i = labels.rows-1 ; i >= 0  ; i--)
    {
        stringstream ss;
        cout << "center :" << i <<  centers.at<float>(i) << "\n" ;
        ss << labels.at<int>(i);
        string str = ss.str();
        chdir(str.c_str());
        stringstream ssfn;
        ssfn << i << ".png";
        string filename = ssfn.str();
        imwrite(filename,source_array.back());
        chdir("..");
        //imshow("Clustered",source_array.back());
        source_array.pop_back();
    }

    cout << "Matrix" << source_array.size();

}
