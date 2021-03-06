// program.cpp
#include <array>
#include <fstream>
#include <iostream>
#include <vector>
#include "dataPoint.h"
#include "kMeans.h"

int main(int argc, char *argv[])
{
    int _originalLabel;
    int _maxLabels = 0;
    std::array<double, 13> _data;
    std::vector<DataPoint> _dataPoints;
    std::ifstream _inFile;

    
    void read_batch(string filename, vector<Mat> &vec, Mat &label){
      ifstream file (filename, ios::binary);
      if (file.is_open())
        {
          int number_of_images = 10000;
          int n_rows = 32;
          int n_cols = 32;
          // for each image in file
          for(int i = 0; i < number_of_images; ++i)
            {
              unsigned char tplabel = 0;
              file.read((char*) &tplabel, sizeof(tplabel));
              vector<Mat> channels;
              Mat fin_img = Mat::zeros(n_rows, n_cols, CV_8UC3);
              for (int ch = 0; ch < 3; ++ch){
                Mat tp = Mat::zeros(n_rows, n_cols, CV_8UC1);
                for(int r = 0; r < n_rows; ++r){
                  for(int c = 0; c < n_cols; ++c){
                    unsigned char temp = 0;
                    file.read((char*) &temp, sizeof(temp));
                    tp.at<uchar>(r, c) = (int) temp;
                  }
                }
                channels.push_back(tp);
              }
              merge(channels, fin_img);
              vec.push_back(fin_img);
              label.ATD(0, i) = (double)tplabel;
            }
        }
    }


    _inFile.open("labeled_frames.txt");
    if (!_inFile)
    {
        std::cout << "Unable to open file";
        exit(1); // terminate with error
    }

    while (_inFile >>
           _originalLabel >>
           _data[0] >>
           _data[1] >>
           _data[2] >>
           _data[3] >>
           _data[4] >>
           _data[5] >>
           _data[6] >>
           _data[7] >>
           _data[8] >>
           _data[9] >>
           _data[10] >>
           _data[11] >>
           _data[12])
    {
        _dataPoints.push_back(DataPoint(_originalLabel, _data));
        if (_originalLabel > _maxLabels)
        {
            _maxLabels = _originalLabel;
        }
    }

    _inFile.close();

    // Run kmeans
    KMeans _kMeans = KMeans(_dataPoints, 10, _maxLabels);
    _kMeans.Run();

    // print output to file
    std::ofstream myfile;
    myfile.open ("output");
    for (int i = 0; i < _dataPoints.size(); i++)
    {
      myfile << _kMeans.DataPoints[i].Label << " " << _kMeans.DataPoints[i].CentroidIndex << std::endl;
    }
    myfile.close();
    return 0;
}
