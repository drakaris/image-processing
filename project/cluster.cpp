using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*This function groups similar faces into clusters */
void ClusterFaces(Mat lbp_array , vector<int> pid , int uid , vector<Mat> filter_array , vector<Mat> thumb_array)
{
    Mat points;
    lbp_array.convertTo(points, CV_32F);                /* 1.Convert into points */
    Mat labels;                                         /* 2.Labels for clusters */
    int clusterCount = 3;                               /* 3.Cluster count */
    Mat centers;                                        /* 4.Centroid array of clusters */
    kmeans(points,clusterCount, labels,TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0),3, KMEANS_PP_CENTERS, centers);
    //cout<<"Center: \n"<<centers<<endl;
    //cout<<"Labels: \n"<<labels<<endl;
    /*
    for(int i = labels.rows-1 ; i >= 0  ; i--)
    {
        stringstream ss;
        ss << labels.at<int>(i);
        string str = ss.str();
        PopulateCluster(str , pid[i] , uid);
    }
    */
    stringstream du;
    du << uid;
    string d;
    d = du.str();
    chdir(d.c_str());
    mkdir("ThumbNails" , S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
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
        path << d <<"/" << str << "/" << filename;
        PopulateCluster(str , pid[i] , uid , path.str());
        chdir("..");
        filter_array.pop_back();
        thumb_array.pop_back();
    }

    cout << "Completed" << endl;
}
