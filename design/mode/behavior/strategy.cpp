#include <iostream>
#include <vector>

// 策略接口
class SortingStrategy {
public:
    virtual void sort(std::vector<int>& data) = 0;
    virtual ~SortingStrategy() = default;
};

// 具体策略
class QuickSort : public SortingStrategy {
public:
    void sort(std::vector<int>& data) override {
        std::cout << "Sorting with QuickSort" << std::endl;
        // 实际实现省略
    }
};

class MergeSort : public SortingStrategy {
public:
    void sort(std::vector<int>& data) override {
        std::cout << "Sorting with MergeSort" << std::endl;
        // 实际实现省略
    }
};

// 上下文
class Sorter {
private:
    SortingStrategy* _strategy;

public:
    Sorter(SortingStrategy* strategy) : _strategy(strategy) {}

    void setStrategy(SortingStrategy* strategy) {
        _strategy = strategy;
    }

    void executeSort(std::vector<int>& data) {
        _strategy->sort(data);
    }
};

int main() {
    std::vector<int> data = {5, 2, 7, 1};
    QuickSort quickSort;
    MergeSort mergeSort;

    Sorter sorter(&quickSort);
    sorter.executeSort(data); // 输出 QuickSort

    sorter.setStrategy(&mergeSort);
    sorter.executeSort(data); // 输出 MergeSort
    return 0;
}