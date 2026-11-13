#pragma once
#include <cstddef>
#include <string>
#include <vector>

class TrieNode
{
public:
    TrieNode *children[26];
    bool endOfWord;
    std::string originalWord;
    int restaurantId;
    TrieNode();
};

class Trie
{
private:
    TrieNode *root;

    void collectWords(TrieNode *node, const std::string &currentWord, std::vector<std::string> &result, std::size_t limit) const;
    void deleteTrie(TrieNode *node);
    std::string normalise(const std::string &value) const;

public:
    Trie();
    ~Trie();
    Trie(const Trie &) = delete;
    Trie &operator=(const Trie &) = delete;

    void insert(const std::string &word, int id = -1);
    bool search(const std::string &word) const;
    bool startWith(const std::string &prefix) const;
    void clear();

    std::vector<std::string> autoComplete(const std::string &prefix, std::size_t limit = 10) const;
};