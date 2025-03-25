/*
    主要改动说明：
        添加归并排序：采用分治法，递归地将数组分成两半分别排序后合并。
        添加堆排序：通过构建最大堆并反复提取最大元素实现排序。
        添加std::sort测试：作为C++标准库的高效排序参考。
        注释冒泡排序：因其在百万数据量下性能极差，默认不测试。

    性能测试逻辑：每个排序算法使用独立的数据副本，确保测试公平性。

    算法复杂度对比：
        快速排序：平均O(n log n)，最坏O(n²)（但随机数据下表现良好）。

        归并排序：稳定O(n log n)，需要额外空间。

        堆排序：O(n log n)，原地排序但常数因子较大。

        std::sort：通常基于内省排序（快速排序+堆排序混合），高效且稳定。

    运行建议：
        若测试时间过长，可将SPEC调整为较小值（如10,000）。

    编译器优化：建议开启编译优化（如-O2）以获得更准确的性能结果。
*/

#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm> // 用于std::sort

using namespace std;

// 冒泡排序（O(n²)）
void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// 快速排序（O(n log n)）
void quickSort(vector<int>& arr, int low, int high) {
    if (low < high) {
        int pivot = arr[high];
        int i = low - 1;
        for (int j = low; j < high; ++j) {
            if (arr[j] <= pivot) {
                i++;
                swap(arr[i], arr[j]);
            }
        }
        swap(arr[i + 1], arr[high]);
        quickSort(arr, low, i);
        quickSort(arr, i + 2, high);
    }
}

// 归并排序的辅助函数
void merge(vector<int>& arr, int left, int mid, int right) {
    vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
    for (int p = 0; p < k; p++) 
        arr[left + p] = temp[p];
}

// 归并排序（O(n log n)）
void mergeSort(vector<int>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

// 堆排序的辅助函数
void heapify(vector<int>& arr, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest])
        largest = left;
    if (right < n && arr[right] > arr[largest])
        largest = right;
    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

// 堆排序（O(n log n)）
void heapSort(vector<int>& arr) {
    int n = arr.size();
    // 构建最大堆
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);
    // 逐个提取元素
    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

static constexpr size_t SPEC = 1'000'000;
static constexpr int MOD = 10000;

int main() {
    // 生成随机测试数据
    vector<int> data(SPEC);
    for (int& num : data) {
        num = rand() % MOD;
    }
    vector<int> data_bubble{data.begin(), data.begin() + SPEC / 10};
    vector<int> data_quick = data;
    vector<int> data_merge = data;
    vector<int> data_heap = data;
    vector<int> data_std = data;

    // 测试快速排序性能
    auto start = chrono::high_resolution_clock::now();
    quickSort(data_quick, 0, data_quick.size() - 1);
    auto end = chrono::high_resolution_clock::now();
    auto diff = end - start;
    cout << "QuickSort Time: " << chrono::duration<double>(diff).count() << "s\n";
    
    // 测试冒泡排序性能
    start = chrono::high_resolution_clock::now();
    bubbleSort(data_bubble);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "BubbleSort Time: " << chrono::duration<double>(diff).count() << "s\n";

    // 测试归并排序性能
    start = chrono::high_resolution_clock::now();
    mergeSort(data_merge, 0, data_merge.size() - 1);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "MergeSort Time: " << chrono::duration<double>(diff).count() << "s\n";

    // 测试堆排序性能
    start = chrono::high_resolution_clock::now();
    heapSort(data_heap);
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "HeapSort Time: " << chrono::duration<double>(diff).count() << "s\n";

    // 测试std::sort性能
    start = chrono::high_resolution_clock::now();
    sort(data_std.begin(), data_std.end());
    end = chrono::high_resolution_clock::now();
    diff = end - start;
    cout << "std::sort Time: " << chrono::duration<double>(diff).count() << "s\n";

    return 0;
}