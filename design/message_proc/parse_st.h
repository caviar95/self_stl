#pragma once

// 选择性解析配置
struct ParseOptions {
    bool need_order_id = true;
    bool need_price = false;
    // ... 其他配置项 ...

    template <typename T>
    void Configure() {
        if constexpr (std::is_same_v<T, OrderTable>) {
            need_price = true;
        }
    }
};

// 示例使用
/*
ParseOptions opts;
opts.Configure<OrderTable>();
auto result = parser.Parse(data, opts);
*/