#include <string>
#include <vector>
#include <iostream>

class BloomFilter {
public:
    BloomFilter(size_t size, size_t cnt) : hashSize(size), hashCnt(cnt), bits_(size)  {}

    void insert(const std::string &item) {
        for (size_t i = 0; i < hashCnt; ++i) {
            bits_[hash_i(item, i) % hashSize] = true;
        }
    }

    bool contains(const std::string &item) {
        for (size_t i = 0; i < hashSize; ++i) {
            if (!bits_[hash_i(item, i) % hashSize]) {
                return false;
            }
        }

        return true;
    }
private:
    size_t hashSize;
    size_t hashCnt;
    std::vector<bool> bits_;

    size_t hash_i(const std::string &item, size_t i) {
        std::hash<std::string> hasher;
        return hasher(item + std::to_string(i));
    }
};

int main()
{
    BloomFilter bf(1000, 5);

    bf.insert("apple");
    bf.insert("banana");

    std::cout << "apple: " << (bf.contains("apple") ? "true" : "false") << std::endl;

    std::cout << "xxx: " << (bf.contains("xxx") ? "true" : "false") << std::endl;

    return 0;
}

