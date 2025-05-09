#pragma once

#include <span>

#include "base_type.h"

class ITableParser {
public:
    virtual ~ITableParser() = default;

    // 解析输入数据并返回解析后的表
    virtual ParsedTable Parse(const std::span<const std::byte> &data) = 0;
};

template <typename TblType>
class TableParser : public ITableParser{
public:
    explicit TableParser(FieldSelect selector = {}) : selector_(std::move(selector)) {}

    ParsedTable Parse(const std::span &data) override {
        TblType tbl;

        if constexpr (has_schema_v<TblType>) {
            // 使用元编程自动生成解析逻辑
            ForEachField(tbl, [&](auto &field) {
                if (selector_.NeedParse(field.id())) {
                    ParseField(data, field);
                }
            });
        } else {
            ParseLegacyTable(data, tbl);
        }

        return PostProcess(tbl);
    }

private:
    FieldSelect selector_;
};