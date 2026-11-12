#include "../../include/storage/BPlusTree.h"

using namespace std;

BPlusTree::Node::Node(bool leaf)
{
    isLeaf = leaf;
    parent = nullptr;
    next = nullptr;
}

BPlusTree::BPlusTree()
{
    root = nullptr;
    itemCount = 0;
}

BPlusTree::~BPlusTree()
{
    clear();
}

BPlusTree::Node *BPlusTree::findLeaf(int key) const
{
    if (root == nullptr)
        return nullptr;
    Node *current = root;
    while (!current->isLeaf)
    {
        int i = 0;
        while (i < current->keys.size() && key >= current->keys[i])
            i++;
        current = current->children[i];
    }
    return current;
}

void BPlusTree::insertInParent(Node *left, int key, Node *right)
{
    if (left == root)
    {
        Node *newRoot = new Node(false);
        newRoot->keys.push_back(key);
        newRoot->children.push_back(left);
        newRoot->children.push_back(right);
        left->parent = newRoot;
        right->parent = newRoot;
        root = newRoot;
        return;
    }
    Node *parent = left->parent;
    int pos = 0;
    while (pos < parent->children.size() && parent->children[pos] != left)
        pos++;
    parent->keys.insert(parent->keys.begin() + pos, key);
    parent->children.insert(parent->children.begin() + pos + 1, right);
    right->parent = parent;
    if (parent->keys.size() >= ORDER)
        splitInternal(parent);
}

void BPlusTree::splitLeaf(Node *leaf)
{
    int mid = leaf->keys.size() / 2;
    Node *right = new Node(true);
    right->parent = leaf->parent;
    for (int i = mid; i < leaf->keys.size(); i++)
    {
        right->keys.push_back(leaf->keys[i]);
        right->values.push_back(leaf->values[i]);
    }
    leaf->keys.resize(mid);
    leaf->values.resize(mid);
    right->next = leaf->next;
    leaf->next = right;
    insertInParent(leaf, right->keys[0], right);
}

void BPlusTree::splitInternal(Node *node)
{
    int mid = node->keys.size() / 2;
    int promote = node->keys[mid];
    Node *right = new Node(false);
    right->parent = node->parent;
    for (int i = mid + 1; i < node->keys.size(); i++)
        right->keys.push_back(node->keys[i]);
    for (int i = mid + 1; i < node->children.size(); i++)
    {
        right->children.push_back(node->children[i]);
        node->children[i]->parent = right;
    }
    node->keys.resize(mid);
    node->children.resize(mid + 1);
    insertInParent(node, promote, right);
}

void BPlusTree::freeNode(Node *node)
{
    if (node == nullptr)
        return;
    if (!node->isLeaf)
    {
        for (Node *child : node->children)
            freeNode(child);
    }
    delete node;
}

bool BPlusTree::insert(int key, long long value)
{
    if (root == nullptr)
    {
        root = new Node(true);
        root->keys.push_back(key);
        root->values.push_back(value);
        itemCount = 1;
        return true;
    }
    Node *leaf = findLeaf(key);
    for (int i = 0; i < leaf->keys.size(); i++)
    {
        if (leaf->keys[i] == key)
        {
            leaf->values[i] = value;
            return true;
        }
    }
    int pos = 0;
    while (pos < leaf->keys.size() && leaf->keys[pos] < key)
        pos++;
    leaf->keys.insert(leaf->keys.begin() + pos, key);
    leaf->values.insert(leaf->values.begin() + pos, value);
    itemCount++;
    if (leaf->keys.size() >= ORDER)
        splitLeaf(leaf);
    return true;
}

bool BPlusTree::search(int key, long long &value) const
{
    Node *leaf =
        findLeaf(key);
    if (leaf == nullptr)
        return false;
    for (int i = 0; i < leaf->keys.size(); i++)
    {
        if (leaf->keys[i] == key)
        {
            value = leaf->values[i];
            return true;
        }
    }
    return false;
}

bool BPlusTree::contains(int key) const
{
    long long value;
    return search(key, value);
}

bool BPlusTree::remove(int key)
{
    Node *leaf =
        findLeaf(key);
    if (leaf == nullptr)
        return false;
    for (int i = 0; i < leaf->keys.size(); i++)
    {
        if (leaf->keys[i] == key)
        {
            leaf->keys.erase(leaf->keys.begin() + i);
            leaf->values.erase(leaf->values.begin() + i);
            itemCount--;
            return true;
        }
    }
    return false;
}

vector<pair<int, long long>> BPlusTree::rangeSearch(int start, int end) const
{
    vector<pair<int, long long>> result;
    Node *leaf =
        findLeaf(start);
    while (leaf != nullptr)
    {
        for (int i = 0; i < leaf->keys.size(); i++)
        {
            if (leaf->keys[i] > end)
                return result;
            if (leaf->keys[i] >= start)
                result.push_back({leaf->keys[i], leaf->values[i]});
        }
        leaf = leaf->next;
    }
    return result;
}

vector<pair<int, long long>> BPlusTree::getAll() const
{
    vector<pair<int, long long>> result;
    if (root == nullptr)
        return result;
    Node *current = root;
    while (!current->isLeaf)
        current = current->children[0];
    while (current != nullptr)
    {
        for (int i = 0; i < current->keys.size(); i++)
        {
            result.push_back(
                {current->keys[i],
                 current->values[i]});
        }
        current = current->next;
    }
    return result;
}

void BPlusTree::clear()
{
    freeNode(root);
    root = nullptr;
    itemCount = 0;
}

size_t BPlusTree::size() const
{
    return itemCount;
}

bool BPlusTree::empty() const
{
    return itemCount == 0;
}