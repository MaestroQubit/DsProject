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
    void collectWords(TrieNode *node, string currentWord, vector<string> &result, int limit);
    void deleteTrie(TrieNode *node);
    string normalise(string s);

public:
    Trie();
    ~Trie();
    void insert(string &word);
    bool search(string &word);
    bool startWith(string &prefix);
    vector<string> autoComplete(string &prefix, int limit = 10);
};