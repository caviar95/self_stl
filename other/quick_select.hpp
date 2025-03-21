#pragma once

#include <vector>

// 快速选择算法
// 时间复杂度：O(n)
// 空间复杂度：O(1)
// 快速选择算法是基于快速排序的选择方法，用于在未完全排序的数组中找到第k小的元素
int QuickSelect(std::vector<int>& nums, int left, int right, int k)
{
    if (left == right) {
        return nums[left];
    }

    int pivot = nums[left];
    int l = left - 1;
    int r = right + 1;

    while (l < r) {
        do {
            l++;
        } while (nums[l] < pivot);

        do {
            r--;
        } while (nums[r] > pivot);

        if (l < r) {
            std::swap(nums[l], nums[r]);
        }
    }

    if (k <= r) {
        return QuickSelect(nums, left, r, k);
    }

    return QuickSelect(nums, r + 1, right, k);
}

#include <vector>

int QuickSelectDecrease(std::vector<int>& nums, int left, int right, int k)
{
    if (left == right) {
        return nums[left];
    }

    int pivot = nums[left];
    int l = left - 1;
    int r = right + 1;

    while (l < r) {
        do {
            l++;
        } while (nums[l] > pivot);

        do {
            r--;
        } while (nums[r] < pivot);

        if (l < r) {
            std::swap(nums[l], nums[r]);
        }
    }

    if (k <= r) {
        return QuickSelectDecrease(nums, left, r, k);
    }

    return QuickSelectDecrease(nums, r + 1, right, k);
}
