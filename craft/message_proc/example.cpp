// ----------------------
// 基础类型定义（新增cassert头文件）
// ----------------------
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <variant>
#include <chrono>
#include <cassert>  // 添加断言支持
#include <cstring>

#include <yaml-cpp/yaml.h>

using namespace std;
using namespace std::chrono;

enum class Endianness { Little, Big };
enum class FieldType { UInt32, Double, String, NestedTable };

struct FieldMeta {
    string name;
    FieldType type;
    size_t offset;
    size_t length;
};

struct TableMeta {
    string table_id;
    uint16_t version;
    Endianness endian;
    vector<FieldMeta> fields;
};

// ----------------------
// 解析器核心框架（修改接口默认参数）
// ----------------------
class ITableParser {
public:
    virtual ~ITableParser() = default;
    virtual unordered_map<string, variant<uint32_t, double, string>> 
        Parse(const uint8_t* data, size_t len, 
              const vector<string>& selected_fields = {}) = 0;  // 添加默认参数
};

class TableParserFactory {
    static unordered_map<string, function<unique_ptr<ITableParser>()>> registry_;

public:
    static void Register(const string& table_id, 
                       function<unique_ptr<ITableParser>()> creator) {
        registry_[table_id] = move(creator);
    }

    static unique_ptr<ITableParser> Create(const string& table_id) {
        return registry_.at(table_id)();
    }
};

unordered_map<string, function<unique_ptr<ITableParser>()>> 
    TableParserFactory::registry_;

// ----------------------
// 元数据加载器（保持不变）
// ----------------------
class MetaLoader {
public:
    static TableMeta Load(const string& table_id) {
        YAML::Node config = YAML::LoadFile("table_meta.yaml");
        
        TableMeta meta;
        meta.table_id = table_id;
        meta.version = config[table_id]["version"].as<uint16_t>();
        meta.endian = config[table_id]["endian"].as<string>() == "little" 
                    ? Endianness::Little : Endianness::Big;

        for (const auto& field : config[table_id]["fields"]) {
            meta.fields.push_back({
                field["name"].as<string>(),
                ParseFieldType(field["type"].as<string>()),
                field["offset"].as<size_t>(),
                field["length"].as<size_t>()
            });
        }
        return meta;
    }

private:
    static FieldType ParseFieldType(const string& type_str) {
        static unordered_map<string, FieldType> type_map = {
            {"uint32", FieldType::UInt32},
            {"double", FieldType::Double},
            {"string", FieldType::String},
            {"nested", FieldType::NestedTable}
        };
        return type_map.at(type_str);
    }
};

// ----------------------
// 通用解析器实现（保持不变）
// ----------------------
template <typename T = void>
class GenericTableParser : public ITableParser {
    TableMeta meta_;
    
public:
    GenericTableParser(const string& table_id) 
        : meta_(MetaLoader::Load(table_id)) {}

    unordered_map<string, variant<uint32_t, double, string>> 
    Parse(const uint8_t* data, size_t len, 
          const vector<string>& selected_fields = {}) override {
        
        unordered_map<string, variant<uint32_t, double, string>> result;
        const auto& fields = GetFieldsToParse(selected_fields);

        for (const auto& field : fields) {
            switch (field.type) {
                case FieldType::UInt32:
                    result[field.name] = ReadUInt32(data + field.offset);
                    break;
                case FieldType::Double:
                    result[field.name] = ReadDouble(data + field.offset);
                    break;
                case FieldType::String:
                    result[field.name] = ReadString(data + field.offset, field.length);
                    break;
                default:
                    throw runtime_error("Unsupported field type");
            }
        }
        return result;
    }

private:
    vector<FieldMeta> GetFieldsToParse(const vector<string>& selected) const {
        if (selected.empty()) return meta_.fields;
        
        vector<FieldMeta> filtered;
        for (const auto& f : meta_.fields) {
            if (find(selected.begin(), selected.end(), f.name) != selected.end()) {
                filtered.push_back(f);
            }
        }
        return filtered;
    }

    uint32_t ReadUInt32(const uint8_t* ptr) const {
        return *reinterpret_cast<const uint32_t*>(ptr);
    }

    double ReadDouble(const uint8_t* ptr) const {
        return *reinterpret_cast<const double*>(ptr);
    }

    string ReadString(const uint8_t* ptr, size_t max_len) const {
        if (max_len == 0) {
            return string(reinterpret_cast<const char*>(ptr));
        }
        return string(reinterpret_cast<const char*>(ptr), max_len);
    }
};

// ----------------------
// 修改后的自动注册宏（移除匿名命名空间）
// ----------------------
#define REGISTER_TABLE_PARSER(table_id) \
class table_id##Parser : public GenericTableParser<> { \
public: \
    table_id##Parser() : GenericTableParser<>(#table_id) {} \
}; \
struct table_id##Registrar { \
    table_id##Registrar() { \
        TableParserFactory::Register( \
            #table_id, \
            []{ return make_unique<table_id##Parser>(); } \
        ); \
    } \
}; \
table_id##Registrar table_id##_instance;

// ----------------------
// 测试用例（参数修正）
// ----------------------
vector<uint8_t> GenerateOrderTableData() {
    vector<uint8_t> data(32);
    *reinterpret_cast<uint32_t*>(data.data()) = 0x12345678;
    *reinterpret_cast<double*>(data.data()+8) = 99.99;
    memcpy(data.data()+16, "AAPL", 4);
    return data;
}

void RunTests() {
    // 注册解析器
    REGISTER_TABLE_PARSER(OrderTable);
    REGISTER_TABLE_PARSER(UserTable);

    // 测试用例1：全字段解析
    {
        auto parser = TableParserFactory::Create("OrderTable");
        auto data = GenerateOrderTableData();
        auto result = parser->Parse(data.data(), data.size(), {});  // 显式传递空参数

        assert(get<uint32_t>(result["order_id"]) == 0x12345678);
        assert(abs(get<double>(result["price"]) - 99.99) < 0.001);
        assert(get<string>(result["symbol"]) == "AAPL");
        
        cout << "Test1: Full parse passed\n";
    }

    // 测试用例2：部分字段解析
    {
        auto parser = TableParserFactory::Create("OrderTable");
        auto data = GenerateOrderTableData();
        auto result = parser->Parse(data.data(), data.size(), {"order_id", "price"});

        assert(result.size() == 2);
        assert(result.find("symbol") == result.end());
        
        cout << "Test2: Partial parse passed\n";
    }

    // 性能测试（保持参数一致）
    {
        const int iterations = 1'000'000;
        auto data = GenerateOrderTableData();
        
        auto start = high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            auto parser = TableParserFactory::Create("OrderTable");
            auto result = parser->Parse(data.data(), data.size(), {});
        }
        auto duration = duration_cast<milliseconds>(
            high_resolution_clock::now() - start);
        
        cout << "Test3: Performance - " 
             << iterations << " iterations took "
             << duration.count() << " ms\n";
    }
}

int main() {
    RunTests();
    return 0;
}