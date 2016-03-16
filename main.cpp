#include <iostream>
#include <stdlib.h>
#include <string>
#include "project/database.cpp"
#include "project/libraries.cpp"
#include "project/detector.cpp"
#include "project/histogram.cpp"
#include "project/cluster.cpp"

using namespace std;
//using namespace cv;


int main(int argc, const char *argv[])
{
  Mat img;
  std::vector<Mat> cropped;
  std::vector<Mat> source_array;
  std::vector<Mat> filtered_array;
  std::vector<Mat> thumbnail_array;
  std::vector<int> source_id_array;
  Mat lbp_array;

  if (argc != 2)
	{
    cout << "Insufficient parameters" << "\n";
    exit(1);
  }
  cout << "Retreiving Data for ID : " << argv[1] << "\n";    
  EstablishConnection("c9","localhost","root","$haringan1208!");
  vector<string> name = FetchData(atoi(argv[1]));
  vector<int> photo_id = FetchID(atoi(argv[1]));
  CascadeClassifier faceDetector = SelectDetector();
  cout << "Images To Be Processed : "  << name.size() << "\n";
  for(int in = 0 ; in < name.size() ; in++)
  {
    img = imread(name[in]);
    // Apply the classifier to the fram
    if (!img.empty())
    {
      cropped = ApplyDetector(img , faceDetector);
      for(int i = 0 ; i < cropped.size() ; i++)
      {
        vector<Mat> cropfilter;
        cropfilter = FilterFaces(img , cropped[i]);
        for(int j = 0 ; j < cropfilter.size() ; j++)
        {
          thumbnail_array.push_back(cropfilter.back());
          cropfilter.pop_back();
          filtered_array.push_back(cropfilter.back());
          cropfilter.pop_back();
          source_array.push_back(cropfilter.back());
          cropfilter.pop_back();
          source_id_array.push_back(photo_id[in]);
        }
      }
    }
    else
    {
      cout << " Image Not Found";
      exit(1);
    }
  }
  //cout << "Source Array Size " << source_array.size() << "\n";
  cout << "Filtered Array size" << filtered_array.size() << "\n";
  for(int i = 0 ; i < source_array.size() ; i++ )
  {
    Mat lbp = Histogram(filtered_array[i],1,8);
    lbp_array.push_back(lbp);
  }
  cout << "LBP Array size" << lbp_array.size() << "\n";
  ClusterFaces(lbp_array , source_id_array , atoi(argv[1]) , filtered_array , thumbnail_array);
	return 0;
}
