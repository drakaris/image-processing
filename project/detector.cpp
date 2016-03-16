using namespace std;
using namespace cv;

CascadeClassifier faceDetector;

Mat ShrinkImage(Mat gray)
{
    const int scaledWidth = 320;
    /* Possibly shrink the image, to run much faster. */
    Mat smallImg;
    float scale = gray.cols / (float) scaledWidth;
    if (gray.cols > scaledWidth)
    {
        // Shrink the image while keeping the same aspect ratio.
        int scaledHeight = cvRound(gray.rows / scale);
        resize(gray, smallImg, Size(scaledWidth, scaledHeight));
    }
    else
    {
        // Access the input directly since it is already small.
        smallImg = gray;
    }

    return smallImg;

}

std::vector<Mat> CropFaces( Mat img , std::vector<Rect> faces)
{
    std::vector<Mat> cropped;
    /* 1.Set Region of Interest */
    string text;
    Mat display_image;
    cvtColor(img, display_image, CV_RGB2GRAY);
    cv::Rect roi_b;
    cv::Rect roi_c;

    size_t ic = 0;                                      /* ic is index of current element */
    int ac = 0;                                         /* ac is area of current element */

    /* 2. Iterate through all Detected Faces */
    for (ic = 0; ic < faces.size(); ic++)          
    {
        Mat crop,res;
        roi_c.x = faces[ic].x;
        roi_c.y = faces[ic].y;
        roi_c.width = (faces[ic].width);
        roi_c.height = (faces[ic].height);
        ac = roi_c.width * roi_c.height;                /* Get the area of Detected face */
        Point pt1(faces[ic].x, faces[ic].y);
        Point pt2((faces[ic].x + faces[ic].height), (faces[ic].y + faces[ic].width));
        rectangle(display_image, pt1, pt2, Scalar(0, 255, 0), 2, 8, 0);  /*draw detected faces in image */
        crop = img(Rect(pt1,pt2));
        resize(crop, res, Size(128, 128), 0, 0, INTER_LINEAR);             /* Uniformly resize all images for preprocessing */
        cropped.push_back(res);
    }

    putText(display_image, text, cvPoint(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
    //namedWindow( "Detected", WINDOW_NORMAL);
    //resizeWindow("Detected", 300, 300);
    //imshow("Detected", display_image);
    //waitKey(0);
    return cropped;
}


std::vector<Mat> ApplyDetector(Mat img , CascadeClassifier fd)
{
    std::vector<Mat> cropped;
    CascadeClassifier faceDetector = fd;
	/* 1.Convert  to Gray Scale */
    Mat gray;
    if (img.channels() == 3)
        cvtColor(img, gray, CV_BGR2GRAY);
    else if (img.channels() == 4)
        cvtColor(img, gray, CV_BGRA2GRAY);
    else
        gray = img;
    //Mat smallImg = ShrinkImage(gray);

    /*  2. Standardize the brightness & contrast,to improve dark images */
    Mat equalizedImg;
    equalizeHist(gray,equalizedImg);  //change to smallimg if required

    /* 3. Apply Detecor */ 

    int flags = CASCADE_SCALE_IMAGE;                    /* Search for many faces */
    Size minFeatureSize(20, 20);                        /* Smallest face size.  */
    float searchScaleFactor = 1.1f;                     /* How many sizes to search. */
    int minNeighbors = 4;                               /* Reliability vs many faces. */
    
    std::vector<Rect> faces;
    faceDetector.detectMultiScale(equalizedImg, faces, searchScaleFactor,minNeighbors,flags,minFeatureSize);

    cropped = CropFaces(img,faces);
    return cropped;
}
