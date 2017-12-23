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
    bool _membershipChange = true;
    int epochNum = 0;
    while (_membershipChange)
    {
        epochNum++;
        std::cout << "Epoch " << epochNum << ": changing memberships" << std::endl;

        ChangeMemberships();
        _membershipChange = HomogenizeClusters();
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
            // first is key (centroid) second is value (size)
            for(std::map<int,int>::iterator it = _labelToAllCentroids[i].begin();
                 it != _labelToAllCentroids[i].end(); 
                 ++it) {
                if (it->second > biggestClusterSize)
                {
                    biggestCluster = it->first;
                    biggestClusterSize = it->second;
                }     
            }
            /*for (int j = 0; j < _labelToAllCentroids[i].size(); j++)
            {
                if (_labelToAllCentroids[i][j] > biggestClusterSize)
                {
                    biggestCluster = j;
                    biggestClusterSize = _labelToAllCentroids[i][j];
                }
            }*/
            // assign biggest cluster as new label for orig label
            _labelToBiggestCentroid[i] = biggestCluster;
        }
        else
        {
            _labelToBiggestCentroid[i] = _labelToAllCentroids[i].begin()->first;
        }
    }

    for (int i = 0; i < DataPoints.size(); i++)
    {
        if (DataPoints[i].CentroidIndex != _labelToBiggestCentroid[DataPoints[i].Label])
        {
            DataPoints[i].CentroidIndex = _labelToBiggestCentroid[DataPoints[i].Label];
            if (DataPoints[i].CentroidIndex != DataPoints[i].PreviousCentroidIndex)
            {
                _membershipChange = true;
            }
        }
    }

    return _membershipChange;
}

// Uses new data to recalcultate centroid points
void KMeans::RecalculateCentroids()
{
    // In case we need to make a new centroid later
    // only used once to initialise (seed) engine
    std::random_device rd;
    // random-number engine used (Mersenne-Twister in this case)
    std::mt19937 rng(rd());
    // guaranteed unbiased
    std::uniform_int_distribution<int> uni(0, DataPoints.size());

    // For each centroid
    for (int i = 0; i < NumClusters; i++)
    {
        int _total = 0;
        std::array<double, 13> _clusterSum;
        for (int i = 0; i < _clusterSum.size(); i++)
        {
            _clusterSum[i] = 0.0;
        }
        
        // For each dataPoint
        for (int j = 0; j < DataPoints.size(); j++)
        {
            /* 
            * If this datapoint is in this centroid, add to running
            * total for each dimension
            */
            if (DataPoints[j].CentroidIndex == i)
            {
                for (int k = 0; k < _clusterSum.size(); k++)
                {
                    _clusterSum[k] += DataPoints[j].Data[k];
                }

                _total++;
            }
        }

        if (_total == 0)
        {
            // Reassign to a random datapoint
            // Ensure we don't get the same starting data point two times
            int _index = uni(rng);

            Centroids[i].Data = DataPoints[_index].Data;
        }
        else
        {
            // Average the dimension values
            for (int k = 0; k < _clusterSum.size(); k++)
            {
                _clusterSum[k] /= _total;
            }
        }
        Centroids[i].Data = _clusterSum;
    }
}
