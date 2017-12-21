// kMeans.h
#ifndef KMEANS_H
#define KMEANS_H

#include "dataPoint.h"

class KMeans
{
  public:
    // Our constructor
    KMeans(std::vector<DataPoint> dataPoints, int numClusters);
    std::vector<DataPoint> DataPoints;
    int NumClusters;

    // save centroid for each cluster
    std::vector<DataPoint> Centroids;

    // Initializes the clusers to random points
    void InitializeCentroids();

    // Reassigns memberships. If anything changes, we return true
    bool ChangeMemberships();

    // Homogenize the clusters so all labels are in the same super cluster
    bool HomogenizeClusters();

    // Uses new data to recalcultate centroid points
    void RecalculateCentroids();

    // Runs the KMeans algorithm
    void Run();
};

#endif
