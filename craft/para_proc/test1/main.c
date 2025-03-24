#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "用法：%s <数量> <字符串1> <字符串2>...\n", argv[0]);
        return 1;
    }

    // 解析数量参数
    int n = atoi(argv[1]);
    if (n <= 0 || argc != n + 2) {
        fprintf(stderr, "错误：无效的数量或参数不匹配。\n");
        return 1;
    }

    // 处理每个字符串
    for (int i = 2; i < argc; ++i) {
        char buffer[256];
        strncpy(buffer, argv[i], sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';

        char *current = buffer;

        // 提取源IP
        char *src_end = strchr(current, ' ');
        if (!src_end) {
            fprintf(stderr, "错误：字符串 %d 格式无效。\n", i - 1);
            continue;
        }
        *src_end = '\0';
        char *src_ip = current;

        // 跳过多余空格到目的IP
        current = src_end + 1;
        while (*current == ' ') current++;
        if (*current == '\0') {
            fprintf(stderr, "错误：字符串 %d 缺少目的IP。\n", i - 1);
            continue;
        }

        // 提取目的IP
        char *dest_end = strchr(current, ' ');
        if (!dest_end) {
            fprintf(stderr, "错误：字符串 %d 缺少城市。\n", i - 1);
            continue;
        }
        *dest_end = '\0';
        char *dest_ip = current;

        // 跳过多余空格到城市
        current = dest_end + 1;
        while (*current == ' ') current++;
        if (*current == '\0') {
            fprintf(stderr, "错误：字符串 %d 城市名为空。\n", i - 1);
            continue;
        }
        char *city = current;

        // 输出结果（可根据需要处理数据）
        printf("字符串 %d:\n", i - 1);
        printf("  源IP: %s\n", src_ip);
        printf("  目的IP: %s\n", dest_ip);
        printf("  城市: %s\n\n", city);
    }

    return 0;
}
