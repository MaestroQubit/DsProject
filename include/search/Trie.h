#pragma once
#include <string>
#include <vector>
using namespace std;

class TrieNode
{
public:
    TrieNode *children[26];
    bool endOfWord;

    TrieNode()
    {
        for (int i = 0; i < 26; i++)
            children[i] = nullptr;
        endOfWord = false;
    }
};

class Trie
{
private:
    TrieNode *root;
    void collectWords(TrieNode *node, string currentWord, vector<string> &result, int limit) const;
    void deleteTrie(TrieNode *node);
    string normalise(string s) const;

public:
    Trie();
    ~Trie();
    void insert(const string &word);
    bool search(const string &word);
    bool startWith(const string &prefix) const;
    vector<string> autoComplete(const string &prefix, int limit = 10) const;
};