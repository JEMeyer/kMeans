// kMeans.cpp
#include <vector>
#include <string>
#include <set>
#include <random>
#include <limits>
#include <iostream>
#include "kMeans.h"
#include "dataPoint.h"
#define MAX_DOUBLE std::numeric_limits<double>::max()

KMeans::KMeans(std::vector<DataPoint> dataPoints, int clusterSize)
{
    DataPoints = dataPoints;
    ClusterSize = clusterSize;
}

// Runs the k-means algorithm on the defined set of data points with the given cluster size
void KMeans::Run()
{
    InitializeClusters();
    bool _membershipChange = true;
    while (_membershipChange)
    {
        _membershipChange = ChangeMemberships();
        RecalculateCentroids();
    }
}

// Initializes the clusers to random points
void KMeans::InitializeClusters()
{
    std::set<int> _usedPointIndexes;
    for (int i = 0; i < ClusterSize; i++)
    {
        // Random number generator from https://stackoverflow.com/questions/5008804/generating-random-integer-from-a-range
        std::random_device rd;     // only used once to initialise (seed) engine
        std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
        std::uniform_int_distribution<int> uni(0, DataPoints.size()); // guaranteed unbiased

        // Ensure we don't get the same starting data point two times
        int _index = uni(rng);
        while (_usedPointIndexes.count(_index) != 0)
        {
            _index = uni(rng);
        }
        _usedPointIndexes.insert(_index);

        // Make a new datapoint for the cluster. 'State' can just be cluster number, we don't really care
        Clusters.push_back(DataPoint(
            DataPoints[_index].Label,
            DataPoints[_index].Data));
    }
}

// Reassigns memberships. If anything changes, we return true
bool KMeans::ChangeMemberships()
{
    bool _membershipChange = false;
    for (int i = 0 ; i < DataPoints.size(); i++)
    {
        // If centroid index is -1 this is the first run, just set to max so we know we'll get assigned one
        double _currentDistance;
        if (DataPoints[i].CentroidIndex == -1)
        {
            _currentDistance = MAX_DOUBLE;
        }
        else
        {
            _currentDistance = DataPoints[i].CalculateDistance(Clusters[DataPoints[i].CentroidIndex]);
        }

        // Loop through all clusters and see if any are explicitly larger.
        for (int j = 0; j < ClusterSize; j++)
        {
            // Calculate potential new distance. If it's lower, it's the newest current and the centroid index changes
            double _potentialNewDistance = DataPoints[i].CalculateDistance(Clusters[j]);
            if (_potentialNewDistance < _currentDistance)
            {
                _currentDistance = _potentialNewDistance;
                DataPoints[i].CentroidIndex = j;
                _membershipChange = true;
            }
        }
    }
    return _membershipChange;
}

// Uses new data to recalcultate centroid points
void KMeans::RecalculateCentroids()
{
    // Loop through each centroid
    for (int i = 0; i < ClusterSize; i++)
    {
        int _total = 0;
        double _murderTotal = 0.0, _assaultTotal = 0.0, _popTotal = 0.0, _rapeTotal = 0.0;

        // Loop through all datapoints to see which ones are attached to this centroid. We will add up all
        // points and then average
        for (int j = 0; j < DataPoints.size(); j++)
        {
            if (DataPoints[j].CentroidIndex == i)
            {
                _total++;
                _murderTotal += DataPoints[j].MurderRate;
                _assaultTotal += DataPoints[j].AssaultRate;
                _popTotal += DataPoints[j].Population;
                _rapeTotal += DataPoints[j].RapeRate;
            }
        }

        // Assign the new averages for this cluster
        Clusters[i].MurderRate = _murderTotal / _total;
        Clusters[i].AssaultRate = _assaultTotal / _total;
        Clusters[i].Population = _popTotal / _total;
        Clusters[i].RapeRate = _rapeTotal / _total;
    }
}

// Gets the total distortion for the space
double KMeans::CalculateDistortion()
{
    double _totalDistortion = 0.0;
    for (int i = 0; i < DataPoints.size(); i++)
    {
        _totalDistortion += DataPoints[i].CalculateDistance(Clusters[DataPoints[i].CentroidIndex]);
    }
    return _totalDistortion;
}