#include <vector>
#include <bitset>
#include <functional>
#include <string>
#include <iostream>

class BloomFilter {
public:
    BloomFilter(size_t size, size_t hashCnt) : size_(size), hashCnt_(hashCnt), bits_(size) {}

    void insert(const std::string &item) {
        for (size_t i = 0; i < hashCnt_; ++i) {
            size_t hash = hash_i(item, i);
            bits_[hash % size_] = true;
        }
    }

    bool contains(const std::string &item) const {
        for (size_t i = 0; i < hashCnt_; ++i) {
            size_t hash = hash_i(item, i);
            if (!bits_[hash % size_]) {
                return false;
            }
        }

        return true;
    }


private:
    size_t size_;
    size_t hashCnt_;
    std::vector<bool> bits_;

    size_t hash_i(const std::string &item, size_t i) const {
        std::hash<std::string> hasher;
        return hasher(item + std::to_string(i));
    }
};

int main() {
    BloomFilter bf(1000, 5);

    bf.insert("apple");
    bf.insert("banana");

    std::cout << "apple: " << bf.contains("apple") << std::endl;
    std::cout << "grape: " << bf.contains("grape") << std::endl;

    return 0;
}