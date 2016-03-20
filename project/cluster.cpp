using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <opencv2/flann/flann.hpp>

/*This function groups similar faces into clusters */
void ClusterFaces(Mat lbp_array , vector<int> pid , int uid , vector<Mat> filter_array , vector<Mat> thumb_array , int k)
{
    Mat points;
    lbp_array.convertTo(points, CV_32F);                /* 1.Convert into points */
    Mat labels;                                         /* 2.Labels for clusters */
    int clusterCount = k;                               /* 3.Cluster count */
    Mat centers;                                        /* 4.Centroid array of clusters */

    /* K Means Clustering */
    kmeans(points,clusterCount, labels,TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0),3, KMEANS_PP_CENTERS, centers);
    
    /* Convert centriod matrix to vector */
    Mat reshaped;
    centers.convertTo(reshaped, CV_32F);


    /* Switch to the user directory */
    stringstream directory;
    directory << uid;
    string dir = directory.str();
    chdir(dir.c_str());
    mkdir("ThumbNails" , S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    /* create cluster folders */
    for(int i = 0 ; i < clusterCount ; i++)
    {
        stringstream foldername;
        foldername << i;
        string fname = foldername.str();
        mkdir(fname.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }


    /* Write ThumbNails and Histograms to file system and populate cluster table */

    for(int i = labels.rows-1 ; i >= 0  ; i--)
    {
        //cout<<"Center: \n"<<reshaped.at<float>(i) <<endl;
        stringstream ss;
        ss << labels.at<int>(i);
        string str = ss.str();
        
        chdir("ThumbNails");
        stringstream thumb;
        thumb << str << ".jpg";
        string fen = thumb.str();
        imwrite(fen,thumb_array.back());
        chdir("..");
        
        chdir(str.c_str());
        stringstream ssfn;
        ssfn << i << ".jpg";
        string filename = ssfn.str();
        imwrite(filename,filter_array.back());
        stringstream path;
        path << dir <<"/" << str << "/" << filename;
        
        PopulateCluster(str , pid[i] , uid , path.str());
        chdir("..");
        
        filter_array.pop_back();
        thumb_array.pop_back();
    }

    cout << "Completed" << endl;
}




/* Test function */

void ClusterFacesOne(Mat lbp_array , vector<int> pid , int uid , vector<Mat> filter_array , vector<Mat> thumb_array)
{
    Mat points;
    lbp_array.convertTo(points, CV_32F);                /* 1.Convert into points */
    int clusterCount = lbp_array.rows;
    double avg_sse_count[clusterCount+1];

    /* K Means Clustering */

    for(int count = 0 ; count < 5 ; count++)
    {
        double sse[clusterCount+1];
        for(int i = 1 ; i <= clusterCount ; i++)
        {
            Mat labels;
            Mat centers;
            kmeans(points,i, labels,TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0),3, KMEANS_PP_CENTERS, centers);
            //cout<< "ClusterCount :   " << i <<" \t Center Rows:   "<<centers.rows<<" \tCenter Columns:   "<<centers.cols<<endl;
            Mat reshaped;
            centers.convertTo(reshaped, CV_32F);
            for(int j = labels.rows-1 ; j >= 0  ; j--)
            {
                for(int n = 0 ; n < points.cols ; n++ )
                    sse[i] += pow(points.at<float>(j,n) - reshaped.at<float>(labels.at<int>(j),n) , 2);
            }
        }
        for(int i = 1 ; i <= clusterCount ; i++)
            avg_sse_count[i] += sse[i];
    }

    //Find the value of K
    for(int i = 1 ; i <= clusterCount ; i++)
    {
        avg_sse_count[i] = avg_sse_count[i]/5.0;
        cout << avg_sse_count[i] << endl;
    }
    double cmp = avg_sse_count[1] - avg_sse_count[2];
	cout << "Difference " << endl << cmp << endl;
    int k = 1;
    for(int i = 2 ; i < clusterCount ; i++)
    {
        double val = avg_sse_count[i] - avg_sse_count[i+1];
	cout << val << endl;
        if(val > cmp)
            k = i;
    }

    cout << "K = " << k << endl ;

    ClusterFaces(lbp_array,pid,uid,filter_array,thumb_array,k);

}

















void HeirarchicalClusters(Mat lbp_array , vector<int> pid , int uid , vector<Mat> filter_array , vector<Mat> thumb_array)
{
    Mat points;
    lbp_array.convertTo(points, CV_32F);
    cout << "In HeirarchicalClusters" << endl;
    Mat1f centers;
    cvflann::KMeansIndexParams k_params(32, 11, cvflann::FLANN_CENTERS_KMEANSPP,0.01);
    int count = cv::flann::hierarchicalClustering<cv::flann::L2<float> >(points,centers,k_params);
    cout << "Count :  " << count << endl;
    
}
