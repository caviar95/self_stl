#include <bits/stdc++.h>
using namespace std;

static bool isIdentStart(char c) {
    return (c == '_') || std::isalpha(static_cast<unsigned char>(c));
}
static bool isIdentChar(char c) {
    return (c == '_') || std::isalnum(static_cast<unsigned char>(c));
}

// 去掉注释与字符串（含原始字符串 R"( ... )"、转义）
static string stripCode(const string& s) {
    enum State { Normal, LineComment, BlockComment, StringLit, CharLit, RawString };
    State st = Normal;
    string out;
    out.reserve(s.size());

    size_t i = 0, n = s.size();
    string rawDelim; // 原始字符串分隔符
    while (i < n) {
        char c = s[i];

        if (st == Normal) {
            if (c == '/' && i + 1 < n && s[i + 1] == '/') {
                st = LineComment; i += 2; continue;
            }
            if (c == '/' && i + 1 < n && s[i + 1] == '*') {
                st = BlockComment; i += 2; continue;
            }
            if (c == 'R' && i + 1 < n && s[i + 1] == '"') {
                // 解析原始字符串分隔符：R"delim(
                size_t j = i + 2;
                while (j < n && s[j] != '(') ++j;
                if (j < n) {
                    rawDelim.assign(s.begin() + i + 2, s.begin() + j);
                    st = RawString;
                    i = j + 1; // 跳过 '('
                    continue;
                }
                // 非法形式，当普通字符处理
            }
            if (c == '"') { st = StringLit; ++i; continue; }
            if (c == '\'') { st = CharLit; ++i; continue; }
            out.push_back(c);
            ++i;
        }
        else if (st == LineComment) {
            if (c == '\n') { st = Normal; out.push_back('\n'); }
            ++i;
        }
        else if (st == BlockComment) {
            if (c == '*' && i + 1 < n && s[i + 1] == '/') {
                st = Normal; i += 2; continue;
            }
            ++i;
        }
        else if (st == StringLit) {
            if (c == '\\') { // 转义
                i += 2;
                continue;
            }
            if (c == '"') { st = Normal; ++i; continue; }
            ++i;
        }
        else if (st == CharLit) {
            if (c == '\\') { i += 2; continue; }
            if (c == '\'') { st = Normal; ++i; continue; }
            ++i;
        }
        else if (st == RawString) {
            // 查找 )delim"
            if (c == ')' && i + rawDelim.size() + 1 < n) {
                if (s.compare(i + 1, rawDelim.size(), rawDelim) == 0 &&
                    s[i + 1 + rawDelim.size()] == '"') {
                    st = Normal;
                    i += 2 + rawDelim.size(); // 跳过 )delim"
                    continue;
                }
            }
            ++i;
        }
    }
    return out;
}

// 按“完整标识符”计数：仅当 token == target 才计入
static int countIdentifierToken(const string& code, const string& target) {
    if (target.empty()) return 0;
    int cnt = 0;
    size_t i = 0, n = code.size();
    while (i < n) {
        if (!isIdentStart(code[i])) { ++i; continue; }
        size_t j = i + 1;
        while (j < n && isIdentChar(code[j])) ++j;
        if (code.compare(i, j - i, target) == 0) ++cnt;
        i = j;
    }
    return cnt;
}

int main(int argc, char** argv) {
    const string filename = __FILE__;
    string target = "int"; // 可用命令行覆盖：./a.out int
    if (argc >= 2) target = argv[1];

    ifstream ifs(filename, ios::in | ios::binary);
    if (!ifs) {
        cerr << "无法打开文件: " << filename << "\n";
        return 1;
    }
    string content((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());

    string cleaned = stripCode(content);
    int occurrences = countIdentifierToken(cleaned, target);

    cout << '"' << target << "\" 出现次数: " << occurrences << "\n";
    return 0;
}
