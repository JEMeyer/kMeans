// kMeans.cpp
#include <array>
#include <vector>
#include <set>
#include <map>
#include <random>
#include <limits>
#include <algorithm>
#include <iostream>
#include "kMeans.h"
#include "dataPoint.h"
#define MAX_DOUBLE std::numeric_limits<double>::max()

KMeans::KMeans(std::vector<DataPoint> dataPoints, int numClusters, int maxLabels)
{
    DataPoints = dataPoints;
    NumClusters = numClusters;
    maxLabels = maxLabels;
}

// Runs the k-means algorithm on the defined set of data points with
// the given cluster size
void KMeans::Run()
{
    InitializeCentroids();
    int _membershipChange = 10001;
    int epochNum = 0;
    while (_membershipChange > 10000)
    {
        epochNum++;
        std::cout << "Epoch " << epochNum << ": changing memberships" << std::endl;

        _membershipChange = ChangeMemberships();
        RecalculateCentroids();
    }
    std::cout << "Completed clustering with " << _membershipChange << " changed frame-label pairs." << std::endl;
}

// Initializes the clusers to guaranteed random points
void KMeans::InitializeCentroids()
{
    std::set<int> _usedPointIndexes;
    for (int i = 0; i < NumClusters; i++)
    {
        // only used once to initialise (seed) engine
        std::random_device rd;
        // random-number engine used (Mersenne-Twister in this case)
        std::mt19937 rng(rd());
        // guaranteed unbiased
        std::uniform_int_distribution<int> uni(0, DataPoints.size());

        // Ensure we don't get the same starting data point two times
        int _index = uni(rng);
        while (_usedPointIndexes.count(_index) != 0)
        {
            _index = uni(rng);
        }
        _usedPointIndexes.insert(_index);

        // Make a new datapoint for the cluster.
        // 'State' can just be cluster number, we don't really care
        Centroids.push_back(DataPoint(
            DataPoints[_index].Label,
            DataPoints[_index].Data));
    }
}

// Reassigns memberships. If any assignment changes, we return true
int KMeans::ChangeMemberships()
{
    int _membershipChange = 0;

    std::map<int, int> my_map;

    // for each DataPoint (DataPoints[i])
    for (int i = 0; i < DataPoints.size(); i++)
    {
        DataPoints[i].PreviousCentroidIndex = DataPoints[i].CentroidIndex;
        double _currentDistance;
        if (DataPoints[i].CentroidIndex == -1)
        {
            // If centroid index == -1, then this is the first run, so we
            // just set _currentDistance to C++ infinity (MAX_DOUBLE, above)
            // so that any new distance will be less than _currentDistance
            _currentDistance = MAX_DOUBLE;
        }
        else
        {
            // _currentDistance is distance between DataPoint[i] and its current
            // centroid (cluster assignment)
            _currentDistance =
                DataPoints[i].CalculateDistance(Centroids[DataPoints[i].CentroidIndex]);
        }

        // For each cluster
        for (int j = 0; j < NumClusters; j++)
        {
            // Calculate distance between DataPoint[i] and Clusters[j]
            double _potentialNewDistance = DataPoints[i].CalculateDistance(Centroids[j]);
            if (_potentialNewDistance < _currentDistance)

            {
                // if DataPoint[i] is closer to Clusters[j] than it was to its
                // past cluster, reassign it
                DataPoints[i].CentroidIndex = j;
                _currentDistance = _potentialNewDistance;
                _membershipChange++;

                // debugging
                //std::cout << DataPoints[i].CentroidIndex << std::endl;
            }
        }

        // if the map doesnt have the key, set to 1
        if (my_map.find(DataPoints[i].CentroidIndex) == my_map.end())
        {
            my_map[DataPoints[i].CentroidIndex] = 1;
        }
        else
        {
            // if the map does have the key, increment
            my_map[DataPoints[i].CentroidIndex] = my_map[DataPoints[i].CentroidIndex] + 1;
        }
    }

    // print out all the <centroidID, numFrames> items
    for (auto item : my_map)
    {
        std::cout << item.first << " " << item.second << "\n";
    }
    return _membershipChange;
}

// Makes the clusters homogenized
bool KMeans::HomogenizeClusters()
{
    bool _membershipChange = false;

    // declare map {orgLabel: {newCluster: numFramesInCluster}}
    std::map<int, std::map<int, int>> _labelToAllCentroids;
    // for each DataPoint
    for (int i = 0; i < DataPoints.size(); i++)
    {
        // if the map doesnt have the nested keys, set to 1
        if (
            _labelToAllCentroids.find(DataPoints[i].Label) == _labelToAllCentroids.end() &&
            _labelToAllCentroids[DataPoints[i].Label].find(DataPoints[i].CentroidIndex) == _labelToAllCentroids[DataPoints[i].Label].end())
        {
            _labelToAllCentroids[DataPoints[i].Label][DataPoints[i].CentroidIndex] = 1;
        }
        else
        {
            // if the map does have the key, increment
            _labelToAllCentroids[DataPoints[i].Label][DataPoints[i].CentroidIndex]++;
        }
    }
    
    std::map<int, int> _labelToBiggestCentroid;
    // for each original label (from before clustering)
    for (int i = 0; i < MaxLabels; i++)
    {
      // check if it's found in more than one cluster
      if (_labelToAllCentroids[i].size() > 1)
        {
          int biggestCluster = -1;
          int biggestClusterSize = -1;
          // for each cluster in which that label is found 
          for (int j = 0; j < _labelToAllCentroids[i].size(); j++)
            {
              if (_labelToAllCentroids[i][j] > biggestClusterSize)
                {
                  biggestCluster = j;
                  biggestClusterSize = _labelToAllCentroids[i][j];
                }
            }
          // assign biggest cluster as new label for orig label
          _labelToBiggestCentroid[i] = biggestCluster;
        }
    }

    
    return _membershipChange;
}

// Uses new data to recalcultate centroid points
void KMeans::RecalculateCentroids()
{
    // For each centroid
    for (int i = 0; i < NumClusters; i++)
    {
        int _total = 0;
        std::array<double, 13> _clusterSum = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                              0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        // For each dataPoint
        for (int j = 0; j < DataPoints.size(); j++)
        {
            /* 
            * If this datapoint is in this centroid, add to running
            * total for each dimension
            TODO: Make generic
            */
            if (DataPoints[j].CentroidIndex == i)
            {
                _clusterSum[0] += DataPoints[j].Data[0];
                _clusterSum[1] += DataPoints[j].Data[1];
                _clusterSum[2] += DataPoints[j].Data[2];
                _clusterSum[3] += DataPoints[j].Data[3];
                _clusterSum[4] += DataPoints[j].Data[4];
                _clusterSum[5] += DataPoints[j].Data[5];
                _clusterSum[6] += DataPoints[j].Data[6];
                _clusterSum[7] += DataPoints[j].Data[7];
                _clusterSum[8] += DataPoints[j].Data[8];
                _clusterSum[9] += DataPoints[j].Data[9];
                _clusterSum[10] += DataPoints[j].Data[10];
                _clusterSum[11] += DataPoints[j].Data[11];
                _clusterSum[12] += DataPoints[j].Data[12];

                _total++;
            }
        }

        // Average the dimension values
        // TODO: Make generic
        _clusterSum[0] /= _total;
        _clusterSum[1] /= _total;
        _clusterSum[2] /= _total;
        _clusterSum[3] /= _total;
        _clusterSum[4] /= _total;
        _clusterSum[5] /= _total;
        _clusterSum[6] /= _total;
        _clusterSum[7] /= _total;
        _clusterSum[8] /= _total;
        _clusterSum[9] /= _total;
        _clusterSum[10] /= _total;
        _clusterSum[11] /= _total;
        _clusterSum[12] /= _total;

        Centroids[i].Data = _clusterSum;
    }
}
