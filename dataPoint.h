// dataPoint.h
#ifndef DATAPOINT_H
#define DATAPOINT_H

class DataPoint
{
    public:
        DataPoint(int label, std::array<double, 13> data);
        int Label;
        std::array<double, 13> Data;
        double CalculateDistance(DataPoint clusterPoint);
        int CentroidIndex;
};

#endif
