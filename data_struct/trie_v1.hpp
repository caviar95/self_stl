#include <vector>
#include <array>
#include <string>

using namespace std;

constexpr int ALPHABET_SIZE = 26;

struct Node {
    array<Node*, ALPHABET_SIZE> data;
    bool isEnd;

    Node() : isEnd(false) {}
};

class Trie {
private:
    Node *root;

public:
    Trie() : root(new Node()) {}

    void insert(const string &word) {
        Node *cur = root;

        for (char i : word) {
            if (!cur->data[i - 'a']) {
                cur->data[i - 'a'] = new Node();
            }

            cur = cur->data[i - 'a'];
        }

        cur->isEnd = true;
    }

    bool search(const string &word) {
        Node *cur = root;

        for (char i : word) {
            if (!cur->data[i - 'a']) {
                return false;
            }

            cur = cur->data[i - 'a'];
        }

        return cur->isEnd;
    }

    bool startsWith(const string &prefix) {
        Node *cur = root;

        for (char i : prefix) {
            if (!cur->data[i - 'a']) {
                return false;
            }

            cur = cur->data[i - 'a'];
        }

        return true;
    }
};