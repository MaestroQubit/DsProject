#pragma once

#include <vector>

class SegmentTree
{
    std::vector<double> sumTree;
    std::vector<double> maxTree;
    std::vector<double> values;

    void build(int node, int start, int end);
    double querySum(int node, int start, int end, int left, int right) const;
    double queryMax(int node, int start, int end, int left, int right) const;
    void updateNode(int node, int start, int end, int index, double value);

public:
    SegmentTree();

    void buildTree(const std::vector<double> &data);
    double rangeSum(int left, int right) const;
    double rangeMax(int left, int right) const;
    void update(int index, double value);
    int size() const;
};