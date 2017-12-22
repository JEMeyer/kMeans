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
      // Print centroids for debugging
      // for (int i = 0; i < NumClusters; i++)
      //   {
      //     DataPoint centroid = Centroids[i];
      //     for (auto const &value : centroid.Data)
      //       {
      //         std::cout << value << " ";
      //       }
      //     std::cout << "\n";
      //   }
      // Done debugging
      
      epochNum++;
      std::cout << "Epoch " << epochNum << ": changing memberships" << std::endl;
      
      _membershipChange = ChangeMemberships();
      RecalculateCentroids();
    }
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
bool KMeans::ChangeMemberships()
{
    bool _membershipChange = false;

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
                _membershipChange = true;
                
                // debugging
                //std::cout << DataPoints[i].CentroidIndex << std::endl;
              }
          }

        if ( my_map.find(DataPoints[i].CentroidIndex) == my_map.end() ) {
          my_map[DataPoints[i].CentroidIndex] = 1;
        } else {
          my_map[DataPoints[i].CentroidIndex] = my_map[DataPoints[i].CentroidIndex]+1;
        }
         
    }

    for(auto elem : my_map)
      {
        std::cout << elem.first << " " << elem.second << "\n";
      }
    return _membershipChange;
}


// Makes the clusters homogenized
bool KMeans::HomogenizeClusters()
{
    bool _membershipChange = false;
    std::map<int, std::map<int, int>> _labelToAllCentroids;
    for (int i = 0; i < DataPoints.size(); i++)
    {
        _labelToAllCentroids[DataPoints[i].Label][DataPoints[i].CentroidIndex]++;
    }

    std::map<int, int> _labelToCentroid;
    for (int i = 0; i < MaxLabels; i++)
    {
        if (_labelToAllCentroids[i].size())
        {
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
        std::array<double, 13> _clusterSum = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
        /*
        for (DataPoint x : DataPoints)
        {
                // add DataPoints to _clusterSum elementwise
                // https://stackoverflow.com/questions/3376124/how-to-add-
                // element-by-element-of-two-stl-vectors
                std::transform(
                    x.begin(),
                    x.end(),
                    .Data.begin(),
                    _clusterSum.begin(),
                    std::subtract<double>());

                // Assign the new averages for this cluster
                std::transform(
                    _clusterSum.begin(),
                    _clusterSum.end(),
                    _clusterSum.begin(),
                    std::bind1st(std::divides<double>(), _total));
                // _total++;
        }
*/
        // For each dataPoint
        for (int j = 0; j < DataPoints.size(); j++)
        {
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
                // add DataPoints to _clusterSum elementwise
                // https://stackoverflow.com/questions/3376124/how-to-add-
                // element-by-element-of-two-stl-vectors
                // std::transform(
                //     _clusterSum.begin(),
                //     _clusterSum.end(),
                //     DataPoints[j].Data.begin(),
                //     _clusterSum.begin(),
                //     std::plus<double>());

                _total++;
            }
        }

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
        // Assign the new averages for this cluster
        // std::transform(
        //     _clusterSum.begin(),
        //     _clusterSum.end(),
        //     Centroids[i].Data.begin(),
        //     std::bind1st(std::divides<double>(), _total));
    }
}
