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
        size_t i = 0;
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
    size_t pos = 0;
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
    size_t mid = leaf->keys.size() / 2;
    Node *right = new Node(true);
    right->parent = leaf->parent;
    for (size_t i = mid; i < leaf->keys.size(); i++)
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
    size_t mid = node->keys.size() / 2;
    int promote = node->keys[mid];
    Node *right = new Node(false);
    right->parent = node->parent;
    for (size_t i = mid + 1; i < node->keys.size(); i++)
        right->keys.push_back(node->keys[i]);
    for (size_t i = mid + 1; i < node->children.size(); i++)
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
    for (size_t i = 0; i < leaf->keys.size(); i++)
    {
        if (leaf->keys[i] == key)
        {
            leaf->values[i] = value;
            return true;
        }
    }
    size_t pos = 0;
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
    for (size_t i = 0; i < leaf->keys.size(); i++)
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

void BPlusTree::updateKeyInAncestors(Node *node, int oldKey, int newKey)
{
    while (node != nullptr && node->parent != nullptr)
    {
        Node *p = node->parent;
        for (size_t i = 0; i < p->keys.size(); i++)
        {
            if (p->keys[i] == oldKey)
            {
                p->keys[i] = newKey;
                return;
            }
        }
        node = p;
    }
}

void BPlusTree::rebalanceInternal(Node *node)
{
    if (node == root)
    {
        if (node->keys.empty())
        {
            if (!node->children.empty())
            {
                root = node->children[0];
                root->parent = nullptr;
                delete node;
            }
            else
            {
                delete root;
                root = nullptr;
            }
        }
        return;
    }

    size_t minKeys = (ORDER - 1) / 2;
    if (node->keys.size() >= minKeys)
        return;

    Node *parent = node->parent;
    size_t pos = 0;
    while (pos < parent->children.size() && parent->children[pos] != node)
        pos++;

    Node *leftSibling = (pos > 0) ? parent->children[pos - 1] : nullptr;
    Node *rightSibling = (pos + 1 < parent->children.size()) ? parent->children[pos + 1] : nullptr;

    if (leftSibling && leftSibling->keys.size() > minKeys)
    {
        int parentKey = parent->keys[pos - 1];
        int borrowKey = leftSibling->keys.back();
        Node *borrowChild = leftSibling->children.back();

        leftSibling->keys.pop_back();
        leftSibling->children.pop_back();

        node->keys.insert(node->keys.begin(), parentKey);
        node->children.insert(node->children.begin(), borrowChild);
        borrowChild->parent = node;

        parent->keys[pos - 1] = borrowKey;
        return;
    }

    if (rightSibling && rightSibling->keys.size() > minKeys)
    {
        int parentKey = parent->keys[pos];
        int borrowKey = rightSibling->keys.front();
        Node *borrowChild = rightSibling->children.front();

        rightSibling->keys.erase(rightSibling->keys.begin());
        rightSibling->children.erase(rightSibling->children.begin());

        node->keys.push_back(parentKey);
        node->children.push_back(borrowChild);
        borrowChild->parent = node;

        parent->keys[pos] = borrowKey;
        return;
    }

    if (leftSibling)
    {
        int parentKey = parent->keys[pos - 1];
        leftSibling->keys.push_back(parentKey);
        for (int k : node->keys)
            leftSibling->keys.push_back(k);
        for (Node *c : node->children)
        {
            leftSibling->children.push_back(c);
            c->parent = leftSibling;
        }

        parent->keys.erase(parent->keys.begin() + (pos - 1));
        parent->children.erase(parent->children.begin() + pos);
        delete node;

        rebalanceInternal(parent);
        return;
    }

    if (rightSibling)
    {
        int parentKey = parent->keys[pos];
        node->keys.push_back(parentKey);
        for (int k : rightSibling->keys)
            node->keys.push_back(k);
        for (Node *c : rightSibling->children)
        {
            node->children.push_back(c);
            c->parent = node;
        }

        parent->keys.erase(parent->keys.begin() + pos);
        parent->children.erase(parent->children.begin() + (pos + 1));
        delete rightSibling;

        rebalanceInternal(parent);
        return;
    }
}

void BPlusTree::rebalanceLeaf(Node *leaf)
{
    if (leaf == root)
    {
        if (leaf->keys.empty())
        {
            delete root;
            root = nullptr;
        }
        return;
    }

    size_t minKeys = ORDER / 2;
    if (leaf->keys.size() >= minKeys)
        return;

    Node *parent = leaf->parent;
    size_t pos = 0;
    while (pos < parent->children.size() && parent->children[pos] != leaf)
        pos++;

    Node *leftSibling = (pos > 0) ? parent->children[pos - 1] : nullptr;
    Node *rightSibling = (pos + 1 < parent->children.size()) ? parent->children[pos + 1] : nullptr;

    if (leftSibling && leftSibling->keys.size() > minKeys)
    {
        int borrowKey = leftSibling->keys.back();
        long long borrowVal = leftSibling->values.back();
        leftSibling->keys.pop_back();
        leftSibling->values.pop_back();

        leaf->keys.insert(leaf->keys.begin(), borrowKey);
        leaf->values.insert(leaf->values.begin(), borrowVal);

        parent->keys[pos - 1] = leaf->keys[0];
        updateKeyInAncestors(leaf, borrowKey, leaf->keys[0]);
        return;
    }

    if (rightSibling && rightSibling->keys.size() > minKeys)
    {
        int borrowKey = rightSibling->keys.front();
        long long borrowVal = rightSibling->values.front();
        rightSibling->keys.erase(rightSibling->keys.begin());
        rightSibling->values.erase(rightSibling->values.begin());

        leaf->keys.push_back(borrowKey);
        leaf->values.push_back(borrowVal);

        parent->keys[pos] = rightSibling->keys[0];
        updateKeyInAncestors(rightSibling, borrowKey, rightSibling->keys[0]);
        return;
    }

    if (leftSibling)
    {
        for (size_t i = 0; i < leaf->keys.size(); i++)
        {
            leftSibling->keys.push_back(leaf->keys[i]);
            leftSibling->values.push_back(leaf->values[i]);
        }
        leftSibling->next = leaf->next;

        parent->keys.erase(parent->keys.begin() + (pos - 1));
        parent->children.erase(parent->children.begin() + pos);
        delete leaf;

        rebalanceInternal(parent);
        return;
    }

    if (rightSibling)
    {
        for (size_t i = 0; i < rightSibling->keys.size(); i++)
        {
            leaf->keys.push_back(rightSibling->keys[i]);
            leaf->values.push_back(rightSibling->values[i]);
        }
        leaf->next = rightSibling->next;

        parent->keys.erase(parent->keys.begin() + pos);
        parent->children.erase(parent->children.begin() + (pos + 1));
        delete rightSibling;

        rebalanceInternal(parent);
        return;
    }
}

bool BPlusTree::remove(int key)
{
    Node *leaf = findLeaf(key);
    if (leaf == nullptr)
        return false;

    int pos = -1;
    for (size_t i = 0; i < leaf->keys.size(); i++)
    {
        if (leaf->keys[i] == key)
        {
            pos = static_cast<int>(i);
            break;
        }
    }
    if (pos == -1)
        return false;

    int oldFirstKey = leaf->keys[0];
    leaf->keys.erase(leaf->keys.begin() + pos);
    leaf->values.erase(leaf->values.begin() + pos);
    itemCount--;

    if (itemCount == 0)
    {
        clear();
        return true;
    }

    if (pos == 0 && !leaf->keys.empty())
    {
        updateKeyInAncestors(leaf, oldFirstKey, leaf->keys[0]);
    }

    rebalanceLeaf(leaf);
    return true;
}

vector<pair<int, long long>> BPlusTree::rangeSearch(int start, int end) const
{
    vector<pair<int, long long>> result;
    Node *leaf =
        findLeaf(start);
    while (leaf != nullptr)
    {
        for (size_t i = 0; i < leaf->keys.size(); i++)
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
        for (size_t i = 0; i < current->keys.size(); i++)
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