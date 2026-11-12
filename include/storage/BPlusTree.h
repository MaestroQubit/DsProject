#pragma once

#include <vector>
#include <utility>
#include <cstddef>

class BPlusTree
{
private:
    struct Node
    {
        bool isLeaf;
        Node *parent;
        Node *next;

        std::vector<int> keys;
        std::vector<long long> values;
        std::vector<Node *> children;

        Node(bool leaf);
    };

    static const int ORDER = 4;

    Node *root;
    std::size_t itemCount;
    Node *findLeaf(int key) const;

    void splitLeaf(Node *leaf);
    void splitInternal(Node *node);
    void insertInParent(Node *left, int key, Node *right);
    void freeNode(Node *node);

public:
    BPlusTree();
    ~BPlusTree();
    BPlusTree(const BPlusTree &) = delete;
    BPlusTree &operator=(const BPlusTree &) = delete;

    bool insert(int key, long long value);
    bool search(int key, long long &value) const;
    bool contains(int key) const;
    bool remove(int key);
    std::vector<std::pair<int, long long>> rangeSearch(int start, int end) const;
    std::vector<std::pair<int, long long>> getAll() const;
    void clear();
    std::size_t size() const;
    bool empty() const;
};