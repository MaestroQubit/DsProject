#include "../../include/search/Trie.h"
#include <algorithm>
#include <cctype>
using namespace std;

TrieNode::TrieNode()
{
    endOfWord = false;
    restaurantId = -1;
    for(int i=0;i<26;i++)
        children[i] = nullptr;
}
Trie::Trie()
{
    root = new TrieNode();
}
Trie::~Trie()
{
    deleteTrie(root);
}

void Trie::deleteTrie(TrieNode *node)
{
    if (node == nullptr)
        return;

    for (int i = 0; i < 26; i++)
        deleteTrie(node->children[i]);

    delete node;
}

void Trie::clear()
{
    deleteTrie(root);
    root = new TrieNode();
}

string Trie::normalise(const string &value) const
{
    string result;
    for (char character : value)
    {
        const unsigned char value = static_cast<unsigned char>(character);
        if ((value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z'))
            result += static_cast<char>(value >= 'A' && value <= 'Z' ? value - 'A' + 'a' : value);
    }
    return result;
}

void Trie::insert(const string &word, int id)
{
    string temp = normalise(word);

    if (temp.empty())
        return;

    TrieNode *current = root;
    int index;

    for (char ch : temp)
    {
        index = ch - 'a';

        if (current->children[index] == nullptr)
            current->children[index] = new TrieNode;

        current = current->children[index];
    }
    current->endOfWord = true;
    current->originalWord = word;
    current->restaurantId = id;
}

bool Trie::search(const string &word) const
{
    string temp = normalise(word);

    if (temp.empty())
        return false;

    TrieNode *current = root;
    int index;

    for (char ch : temp)
    {
        index = ch - 'a';

        if (current->children[index] == nullptr)
            return false;

        current = current->children[index];
    }
    return current->endOfWord;
}

bool Trie::startWith(const string &prefix) const
{
    string temp = normalise(prefix);

    TrieNode *current = root;
    int index;

    for (char ch : temp)
    {
        index = ch - 'a';

        if (current->children[index] == nullptr)
            return false;

        current = current->children[index];
    }
    return true;
}

void Trie::collectWords(TrieNode *node, const string &currentWord, vector<string> &result, size_t limit) const
{
    if (node == nullptr || result.size() >= limit)
        return;

    if (node->endOfWord)
    {
        result.push_back(!node->originalWord.empty() ? node->originalWord : currentWord);
        if (result.size() >= limit)
            return;
    }

    for (int i = 0; i < 26; ++i)
    {
        if (node->children[i] != nullptr)
        {
            const char character = static_cast<char>('a' + i);
            collectWords(node->children[i], currentWord + character, result, limit);
            if (result.size() >= limit)
                return;
        }
    }
}

vector<string> Trie::autoComplete(const string &prefix, size_t limit) const
{
    if (limit == 0)
        return {};
    string temp = normalise(prefix);

    TrieNode *current = root;
    int index;

    for (char ch : temp)
    {
        index = ch - 'a';
        if (current->children[index] == nullptr)
            return {};
        current = current->children[index];
    }

    vector<string> result;

    collectWords(current, temp, result, limit);
    sort(result.begin(), result.end());

    return result;
}