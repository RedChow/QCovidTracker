#include "algorithms.h"
#include <QDebug>

algorithms::algorithms()
{

}

std::unique_ptr<std::vector<int>> algorithms::differences(std::vector<int> &v) {
    std::unique_ptr<std::vector<int>> diff(new std::vector<int>);
    diff->push_back(0);
    for(std::size_t i = 1; i < v.size(); ++i) {
        diff->push_back(v[i] - v[i-1]);
    }

    return diff;
}

std::unique_ptr<std::vector<qreal>> algorithms::movingAverage(std::unique_ptr<std::vector<int>> &v, int days) {
    /*
     * 	This is usually "fast enough" since we're not dealing with large sets of data.
     * 	I usually define large in the amount of time it takes. I.e., if we notice
     * 	this method taking too long to our liking, we should investigate other
     * 	ways of computing the moving average, e.g. the convolution of two 1-d vectors.
     * 	No since in getting too complex too early in the program since other methods may be
     *  particulaly faster for larger sets of data and slower for smaller sets. That would take
     * 	some experimentation and lots of reading.
     */
    std::unique_ptr<std::vector<double>> moving_average(new std::vector<double>);
    moving_average->push_back(0);
    unsigned long daysL = (unsigned long) days;
    std::vector<double> cumulativeSum;
    cumulativeSum.push_back(0);
    double numerator{0};
    double denominator{1};
    for (std::size_t i = 1; i < v->size(); ++i) {
        cumulativeSum.push_back(cumulativeSum[i-1] + (double) v->at(i));
        if (i >= daysL) {
            numerator = (double) (cumulativeSum[i] - cumulativeSum[i-daysL]);
            denominator = (double) days;
            moving_average->push_back(numerator/denominator);
        }
        else {
            moving_average->push_back((double) v->at(i) );
        }
    }
    return moving_average;
}
