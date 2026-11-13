#include "../../include/analytics/SegmentTree.h"
#include <algorithm>

using namespace std;

SegmentTree::SegmentTree() {}

void SegmentTree::buildTree(const vector<double> &data)
{
    values = data;
    if (values.empty())
    {
        sumTree.clear();
        maxTree.clear();
        return;
    }
    int n = values.size();
    sumTree.assign(4 * n, 0.0);
    maxTree.assign(4 * n, 0.0);
    build(1, 0, n - 1);
}

void SegmentTree::build(int node, int start, int end)
{
    if (start == end)
    {
        sumTree[node] = values[start];
        maxTree[node] = values[start];
        return;
    }
    int mid = (start + end) / 2;
    build(node * 2, start, mid);
    build(node * 2 + 1, mid + 1, end);
    sumTree[node] = sumTree[node * 2] + sumTree[node * 2 + 1];
    maxTree[node] = max(maxTree[node * 2], maxTree[node * 2 + 1]);
}

double SegmentTree::querySum(int node, int start, int end, int left, int right) const
{
    if (right < start || end < left)
        return 0.0;
    if (left <= start && end <= right)
        return sumTree[node];
    int mid = (start + end) / 2;
    return querySum(node * 2, start, mid, left, right) + querySum(node * 2 + 1, mid + 1, end, left, right);
}

double SegmentTree::queryMax(int node, int start, int end, int left, int right) const
{
    if (right < start || end < left)
        return -1e18;
    if (left <= start && end <= right)
        return maxTree[node];
    int mid = (start + end) / 2;
    return max(queryMax(node * 2, start, mid, left, right), queryMax(node * 2 + 1, mid + 1, end, left, right));
}

double SegmentTree::rangeSum(int left, int right) const
{
    if (values.empty() || left < 0 || right >= static_cast<int>(values.size()) || left > right)
        return 0.0;
    return querySum(1, 0, values.size() - 1, left, right);
}

double SegmentTree::rangeMax(int left, int right) const
{
    if (values.empty() || left < 0 || right >= static_cast<int>(values.size()) || left > right)
        return 0.0;
    return queryMax(1, 0, values.size() - 1, left, right);
}

void SegmentTree::updateNode(int node, int start, int end, int index, double value)
{
    if (start == end)
    {
        sumTree[node] = value;
        maxTree[node] = value;
        return;
    }
    int mid = (start + end) / 2;
    if (index <= mid)
        updateNode(node * 2, start, mid, index, value);
    else
        updateNode(node * 2 + 1, mid + 1, end, index, value);
    sumTree[node] = sumTree[node * 2] + sumTree[node * 2 + 1];
    maxTree[node] = max(maxTree[node * 2], maxTree[node * 2 + 1]);
}

void SegmentTree::update(int index, double value)
{
    if (values.empty() || index < 0 || index >= static_cast<int>(values.size()))
        return;
    values[index] = value;
    updateNode(1, 0, values.size() - 1, index, value);
}

int SegmentTree::size() const
{
    return values.size();
}