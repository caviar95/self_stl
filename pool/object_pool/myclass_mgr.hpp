// myclass_mgr.hpp
#include "singleton.hpp"
#include "base_object_mgr.hpp"
#include "myclass.hpp"

constexpr size_t MyClassPoolSize = 3;

class MyClassMgr : public Singleton<MyClassMgr>, public BaseObjectMgr<MyClass, MyClassPoolSize> {
protected:
    // 示例：添加私有检查逻辑
    bool preAddCheck(const MyClass& t) override {
        // 这里可以添加具体的检查逻辑，例如检查某个属性值
        return t.getValue() > 0;
    }

    // 可以按需重写其他 preXXXCheck 函数
};
    