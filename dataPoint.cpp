// dataPoint.cpp

#include <array>
#include <string>
#include <cmath>
#include <iostream>
#include "dataPoint.h"

// Our constructor
DataPoint::DataPoint(int label, std::array<double, 13> data)
{
    Label = label;
    Data = data;
    CentroidIndex = -1;
}

// Calculate the euclidian distance between this point and one other point
double DataPoint::CalculateDistance(DataPoint clusterPoint)
{
    // Get the sum of the squares of the distances (differences) on each dimension
    double _sumOfSquaresOfDistance = 0.0;
    for (int i = 0; i < 13; i++)
    {
        _sumOfSquaresOfDistance += std::pow(Data[i] - clusterPoint.Data[i], 2);
    }

    // Return the square root of this overall sum for the euclidian distance
    return std::sqrt(_sumOfSquaresOfDistance);
}