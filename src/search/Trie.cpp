#include "../../include/search/Trie.h"
#include <algorithm>
#include <cctype>
using namespace std;

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

string Trie::normalise(string s) const
{
    string result;

    for (char ch : s)
    {
        if (isalpha(ch))
            result += tolower(ch);
    }
    return result;
}

void Trie::insert(const string &word)
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
}

bool Trie::search(const string &word)
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

void Trie::collectWords(TrieNode *node, string currentWord, vector<string> &result, int limit) const
{
    if (result.size() >= limit)
        return;

    if (node == nullptr)
        return;

    if (node->endOfWord)
    {
        result.push_back(currentWord);
        if (result.size() >= limit)
            return;
    }
    for (int i = 0; i < 26; i++)
    {
        if (node->children[i] != nullptr)
        {
            char ch = 'a' + i;

            collectWords(node->children[i], currentWord + ch, result, limit);
        }
    }
}

vector<string> Trie::autoComplete(const string &prefix, int limit) const
{
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