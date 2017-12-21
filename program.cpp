// program.cpp
#include <array>
#include <fstream>
#include <iostream>
#include <vector>
#include "dataPoint.h"
#include "kMeans.h"

int main(int argc, char* argv[])
{
    int _originalLabel;
    std::array<double,13> _data;
    std::vector<DataPoint> _dataPoints;
    std::ifstream _inFile;
    _inFile.open("labeled_frames.txt");
    if (!_inFile) {
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
    }
    std::cout << "we read in " << _dataPoints.size() << " data points" << std::endl;
    _inFile.close();

    KMeans(_dataPoints, 100).Run();
}
