#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>

struct ConnectionInfo {
    std::string src_ip;
    std::string dest_ip;
    std::string city;
};

int main(int argc, char* argv[]) {
    // 参数校验
    if (argc < 2) {
        std::cerr << "用法: " << argv[0] << " <数量> <字符串1> <字符串2>...\n";
        return EXIT_FAILURE;
    }

    // 解析数量参数
    int num = 0;
    try {
        num = std::stoi(argv[1]);
    } catch (...) {
        std::cerr << "错误: 无效的数量参数\n";
        return EXIT_FAILURE;
    }

    if (num <= 0 || argc != num + 2) {
        std::cerr << "错误: 参数数量不匹配\n";
        return EXIT_FAILURE;
    }

    std::vector<ConnectionInfo> connections;
    
    // 处理每个字符串
    for (int i = 2; i < argc; ++i) {
        std::istringstream iss(argv[i]);
        ConnectionInfo info;
        
        // 提取源IP
        if (!(iss >> info.src_ip)) {
            std::cerr << "警告: 跳过无效字符串 " << i-1 << " (缺少源IP)\n";
            continue;
        }

        // 提取目的IP
        if (!(iss >> info.dest_ip)) {
            std::cerr << "警告: 跳过无效字符串 " << i-1 << " (缺少目的IP)\n";
            continue;
        }

        // 提取城市（允许包含空格）
        std::getline(iss >> std::ws, info.city);
        if (info.city.empty()) {
            std::cerr << "警告: 跳过无效字符串 " << i-1 << " (缺少城市)\n";
            continue;
        }

        connections.push_back(info);
    }

    // 输出结果
    std::cout << "成功解析 " << connections.size() << " 条记录:\n";
    for (size_t idx = 0; idx < connections.size(); ++idx) {
        std::cout << "记录 " << idx+1 << ":\n"
                  << "  源IP: " << connections[idx].src_ip << "\n"
                  << "  目的IP: " << connections[idx].dest_ip << "\n"
                  << "  城市: " << connections[idx].city << "\n\n";
    }

    return EXIT_SUCCESS;
}
