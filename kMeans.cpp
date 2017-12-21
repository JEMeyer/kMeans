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


// Runs the k-means algorithm on the defined set of data points with
// the given cluster size
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


// Initializes the clusers to guaranteed random points
void KMeans::InitializeClusters()
{
    std::set<int> _usedPointIndexes;
    for (int i = 0; i < ClusterSize; i++)
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
      Clusters.push_back(DataPoint(
                                   DataPoints[_index].Label,
                                   DataPoints[_index].Data)
                         );
    }
}


// Reassigns memberships. If any assignment changes, we return true
bool KMeans::ChangeMemberships()
{
  bool _membershipChange = false;
  // for each DataPoint (DataPoints[i])
  for (int i = 0 ; i < DataPoints.size(); i++)
    {
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
          _currentDistance = DataPoints[i].CalculateDistance(Clusters[DataPoints[i].CentroidIndex]);
        }
      
      // For each cluster
      for (int j = 0; j < ClusterSize; j++)
        {
          // Calculate distance between DataPoint[i] and Clusters[j]
          double _potentialNewDistance = DataPoints[i].CalculateDistance(Clusters[j]);
          if (_potentialNewDistance < _currentDistance)
            {
              // if DataPoint[i] is closer to Clusters[j] than it was to its
              // past cluster, reassign it
              DataPoints[i].CentroidIndex = j;
              _currentDistance = _potentialNewDistance;
              _membershipChange = true;
            }
        }
    }
  return _membershipChange;
}


// Uses new data to recalcultate centroid points
void KMeans::RecalculateCentroids()
{
    // For each centroid
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
