#pragma once

#include "cache_policy_intf.h"

namespace KamaCache {

template <typename Key, typename Value> class LruCache;

template <typename Key, typename Value>
class LruNode {
public:
    LruNode(Key key, Value value)
        : key_(key), value_(value), accessCount_(1), prev_(nullptr), next_(nullptr) {}

    Key getKey() const { return key_; }
    Value getValue() const { return value_; }
    void setValue(const Value &value) { value_ = value; }
    size_t getAccessCount() const { return accessCount_; }
    void incrementAccessCount() { accessCount_++; }

    friend class LruCache<Key, Value>;

private:
    Key key_;
    Value value_;
    size_t accessCount_;
    std::shared_ptr<LruNode<Key, Value>> prev_;
    std::shared_ptr<LruNode<Key, Value>> next_;
};
}