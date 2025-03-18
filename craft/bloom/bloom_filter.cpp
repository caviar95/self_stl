#include <iostream>
#include <vector>
#include <functional> // for std::hash
#include <string>

class BloomFilter {
public:
    BloomFilter(size_t size, size_t hash_count);
    void add(const std::string& item);
    bool check(const std::string& item) const;

private:
    std::vector<bool> bit_array;
    size_t size;
    size_t hash_count;

    // 简单的哈希函数集合，这里使用std::hash的修改版本来生成不同的哈希值
    std::vector<std::function<size_t(const std::string&)>> hash_functions;

    // 辅助函数，用于生成一组哈希函数
    void generateHashFunctions();
};

BloomFilter::BloomFilter(size_t size, size_t hash_count)
    : size(size), hash_count(hash_count), bit_array(size, false) {
    generateHashFunctions();
}

void BloomFilter::generateHashFunctions() {
    hash_functions.reserve(hash_count);
    for (size_t i = 0; i < hash_count; ++i) {
        // 使用std::hash作为基础，但通过添加一个种子值来使其变得不同
        // 注意：这只是一个简单的示例，实际应用中可能需要更复杂的哈希函数集合
        hash_functions.emplace_back([i](const std::string& item) {
            std::hash<std::string> hasher;
            size_t hash_value = hasher(item) + i; // 添加种子值i
            return hash_value % size; // 对位数组大小取模
        });
    }
}

void BloomFilter::add(const std::string& item) {
    for (const auto& hash_func : hash_functions) {
        size_t index = hash_func(item);
        bit_array[index] = true;
    }
}

bool BloomFilter::check(const std::string& item) const {
    for (const auto& hash_func : hash_functions) {
        size_t index = hash_func(item);
        if (!bit_array[index]) {
            return false;
        }
    }
    return true;
}

int main() {
    // 初始化布隆过滤器，设置位数组大小为1000，哈希函数数量为7
    BloomFilter bloom(1000, 7);

    // 添加元素
    bloom.add("hello");
    bloom.add("world");

    // 检查元素
    std::cout << std::boolalpha; // 以布尔值形式输出
    std::cout << "hello: " << bloom.check("hello") << std::endl;  // 输出: true
    std::cout << "world: " << bloom.check("world") << std::endl;  // 输出: true
    std::cout << "python: " << bloom.check("python") << std::endl; // 输出: false（假设“python”没有被添加过）

    return 0;
}