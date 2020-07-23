#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <QObject>
#include <memory>
#include <vector>

class algorithms : public QObject
{
    Q_OBJECT
public:
    algorithms();
    static std::unique_ptr<std::vector<int>> differences(std::vector<int> &v);
    static std::unique_ptr<std::vector<qreal>> movingAverage(std::unique_ptr<std::vector<int>> &v, int days);
};

#endif // ALGORITHMS_H
