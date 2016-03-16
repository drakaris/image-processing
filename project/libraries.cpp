
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>
using namespace std;
using namespace cv;



/*Global Declarations here */
CascadeClassifier mcs_lefteye,mcs_righteye;
CascadeClassifier left_eye_split,right_eye_split;
CascadeClassifier default_eye,tree_eyeglasses;
/*End of Global Declarations */

/*This Function selects the face_detector */

CascadeClassifier SelectDetector()
{
	CascadeClassifier faceDetector;
	string faceCascadeFilename;
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

    return faceDetector;
}



/*This function loads the Eye Detectors */
void LoadEyeDetectors()
{
    if( !mcs_lefteye.load("lib/haarcascade_mcs_lefteye.xml") ){ printf("--(!)Error loading Left eyes cascade\n"); exit(0); };
    if( !mcs_righteye.load("lib/haarcascade_mcs_righteye.xml") ){ printf("--(!)Error loading Right eyes cascade\n"); exit(0); };
    if( !left_eye_split.load( "lib/haarcascade_lefteye_2splits.xml" ) ){ printf("--(!)Error loading Left eyes split cascade\n"); exit(0); };
    if( !right_eye_split.load( "lib/haarcascade_righteye_2splits.xml" ) ){ printf("--(!)Error loading Right eyes split cascade\n"); exit(0); };
    if( !default_eye.load( "lib/haarcascade_eye.xml" ) ){ printf("--(!)Error loading eyes cascade\n"); exit(0); };
    if( !tree_eyeglasses.load( "lib/haarcascade_eye_tree_eyeglasses.xml" ) ){ printf("--(!)Error loading Glass eyes cascade\n"); exit(0); };
}

/*This function applies left and right eye detectors individually to the cropped image
    if both detectors return true then the function returns true */

int LeftEyeDetectors(Mat frame , Mat frame_gray)
{
    std::vector<Rect> eyes;
    std::vector<Rect> eyes_right;

    left_eye_split.detectMultiScale( frame_gray, eyes, 1.1, 2, 0 |CASCADE_FIND_BIGGEST_OBJECT | CASCADE_DO_ROUGH_SEARCH, Size(30, 30));
    if(eyes.size() > 0)
    {
        right_eye_split.detectMultiScale( frame_gray, eyes_right, 1.1, 2, 0 |CASCADE_FIND_BIGGEST_OBJECT | CASCADE_DO_ROUGH_SEARCH, Size(30, 30));
        if(eyes_right.size() > 0)
        {
            for( size_t j = 0; j < eyes_right.size(); j++ )
            {
                Point eye_center( eyes_right[j].x + eyes_right[j].width/2, eyes_right[j].y + eyes_right[j].height/2 );
                int radius = cvRound( (eyes_right[j].width + eyes_right[j].height)*0.25 );
                //circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
            }
            for( size_t j = 0; j < eyes.size(); j++ )
            {
                Point eye_center( eyes[j].x + eyes[j].width/2, eyes[j].y + eyes[j].height/2 );
                int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
                //circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
            }
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;
}

/*This function applies mcs left and mcs right eye detectors individually to the cropped image
    if both detectors return true then the function returns true */
int McsLeftEyeDetectors(Mat frame , Mat frame_gray)
{
    std::vector<Rect> eyes;
    std::vector<Rect> eyes_right;

    mcs_lefteye.detectMultiScale( frame_gray, eyes, 1.1, 2, 0 |CASCADE_FIND_BIGGEST_OBJECT | CASCADE_DO_ROUGH_SEARCH, Size(30, 30));
    if(eyes.size() > 0)
    {
        mcs_righteye.detectMultiScale( frame_gray, eyes_right, 1.1, 2, 0 |CASCADE_FIND_BIGGEST_OBJECT | CASCADE_DO_ROUGH_SEARCH, Size(30, 30));
        if(eyes_right.size() > 0)
        {
            for( size_t j = 0; j < eyes_right.size(); j++ )
            {
                Point eye_center( eyes_right[j].x + eyes_right[j].width/2, eyes_right[j].y + eyes_right[j].height/2 );
                int radius = cvRound( (eyes_right[j].width + eyes_right[j].height)*0.25 );
                //circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
            }
            for( size_t j = 0; j < eyes.size(); j++ )
            {
                Point eye_center( eyes[j].x + eyes[j].width/2, eyes[j].y + eyes[j].height/2 );
                int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
                //circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
            }
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;
}

/*This function applies default eye detectors individually to the cropped image
    if both detectors return true then the function returns true */

int DefaultEyeDetectors(Mat frame , Mat frame_gray)
{
    std::vector<Rect> eyes;
    default_eye.detectMultiScale( frame_gray, eyes, 1.1, 2, 0 |CASCADE_FIND_BIGGEST_OBJECT , Size(30, 30));
    if(eyes.size() >= 1)
    {
        for( size_t j = 0; j < eyes.size(); j++ )
        {
            Point eye_center( eyes[j].x + eyes[j].width/2, eyes[j].y + eyes[j].height/2 );
            int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
            //circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
        }
        return 1;
    }
    else 
        return 0;
}

/*This function applies glass eye detectors individually to the cropped image
    if both detectors return true then the function returns true */

int GlassEyeDetectors(Mat frame , Mat frame_gray)
{
    std::vector<Rect> eyes;
    tree_eyeglasses.detectMultiScale( frame_gray, eyes, 1.1, 2, 0 |CASCADE_FIND_BIGGEST_OBJECT , Size(30, 30));
    if(eyes.size() >= 1)
    {
        for( size_t j = 0; j < eyes.size(); j++ )
        {
            Point eye_center( eyes[j].x + eyes[j].width/2, eyes[j].y + eyes[j].height/2 );
            int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
            //circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
        }
        return 1;
    }
    return 0;
}



/* This function is used to remove the false positives by applying an eye detector */

vector<Mat> FilterFaces(Mat img , Mat frame)
{
    vector<Mat> sc_img;
    LoadEyeDetectors();
    std::vector<Rect> eyes;
    std::vector<Rect> eyes_right;
    Mat frame_gray;
    int face_flag;

    /* 1. Convert the cropped image to Gray scale */
    if (frame.channels() == 3)
        cvtColor(frame, frame_gray, CV_BGR2GRAY);
    else if (frame.channels() == 4)
        cvtColor(frame, frame_gray, CV_BGRA2GRAY);
    else
        frame_gray = frame;

    /* 2. Apply Histogram to improve the quality of Dark Images */
    equalizeHist( frame_gray, frame_gray );

    /* Apply Eye Detectors to remove false positives */

    face_flag = LeftEyeDetectors(frame , frame_gray);
    if(face_flag == 0)
        face_flag = McsLeftEyeDetectors(frame , frame_gray);
    if(face_flag == 0)
        face_flag = DefaultEyeDetectors(frame , frame_gray);
    if(face_flag == 0)
        face_flag = GlassEyeDetectors(frame , frame_gray);

    if(face_flag == 1)
    {
        /* 1. Cropped Images after false Positives */
        Mat bilateral_img;
        bilateralFilter(frame_gray,bilateral_img,0, 20.0, 2.0,BORDER_DEFAULT );
        sc_img.push_back(img);
        sc_img.push_back(bilateral_img);
        sc_img.push_back(frame);
        //histogram(bilateral_img,1,8);
    }
    return sc_img;
}
