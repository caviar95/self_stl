#pragma once

/*
    对于空间，
        如果使用堆存储，若有n个叶子节点，则d数组的最大范围是2^(ceil(lg n) + 1)
        也可以直接将数组长度设置为4n

    堆式存储存在无用的叶子节点，可考虑使用内存池管理线段树节点，每当需要新建节点时，从池中获取。
    自底向上考虑，必有每两个底层节点合并为一个上层节点，因此可以类似哈夫曼树地证明，如果有n个叶子节点，这样的线段树总共有2n-1个节点，其空间效率优于堆式存储，并且是可能的最优情况

*/

// 针对区间[s, t], root:p
void build(int s, int t, int p)
{
    if (s == t) {
        d[p] = a[s];
        return;
    }

    int m = s + ((t - s) >> 1);
    build(s, m, p * 2);
    build(m + 1, t, p * 2 + 1);
    d[p] = d[p * 2] + d[p * 2 + 1];
}

// 区间
int getSum(int l, int r, int s, int t, int p)
{
    // [l, r]查询区间，[s, t]当前节点包含的区间，p当前节点的编号

    // 当前区间为询问区间的子集时，直接返回当前区间的和
    if (l <= s && t <= r) {
        return d[p];
    }

    int m = s + ((t - s) >> 1);
    int sum = 0;

    // 如果左儿子代表的区间[s, m]与询问区间有交集
    if (l <= m) {
        sum += getSum(l, r, s, m, p * 2);
    }

    // 如果右儿子代表的区间[m+1, t]与询问区间有交集
    if (r > m) {
        sum += getSum(l, r, m + 1, t, p * 2 + 1);
    }

    return sum;
}

