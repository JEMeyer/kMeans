// dataPoint.h
#ifndef DATAPOINT_H
#define DATAPOINT_H

class DataPoint
{
    public:
        DataPoint(int label, std::array<double, 13> data);
        // our labels are integers coming from frame-to-HMMtransition alignments
        // where the integer is the ID of the transition
        int Label;
        // 13-dimensional PLP features (one frame) from audio
        std::array<double, 13> Data;
        // the index of the cluster this frame was assigned to
        int CentroidIndex;
        // the distance from this frame to it 
        double CalculateDistance(DataPoint clusterPoint);
};

#endif
