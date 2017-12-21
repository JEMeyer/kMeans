// kMeans.h
#ifndef KMEANS_H
#define KMEANS_H

#include "dataPoint.h"

class KMeans
{
    public:
        KMeans(std::vector<DataPoint> dataPoints, int clusterSize); // Our constructor
        int ClusterSize;
        std::vector<DataPoint> DataPoints;
        std::vector<DataPoint> Clusters;
        void Run(); // Runs the KMeans algorithm
        void InitializeClusters(); // Initializes the clusers to random points
        bool ChangeMemberships(); // Reassigns memberships. If anything changes, we return true
        void RecalculateCentroids(); // Uses new data to recalcultate centroid points
};

#endif