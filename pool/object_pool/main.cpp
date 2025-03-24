// main.cpp
#include "myclass_mgr.hpp"
#include <iostream>

int main() {
    auto& mgr = MyClassMgr::getInstance();

    MyClass objToAdd;
    objToAdd.setValue(10);
    size_t idx = mgr.Add(objToAdd);
    if (idx != static_cast<size_t>(-1)) {
        std::cout << "添加对象成功，索引为: " << idx << std::endl;
    }

    const MyClass& retrievedObj = mgr.Get(idx);
    std::cout << "获取对象，id: " << retrievedObj.getId() << ", 值: " << retrievedObj.getValue() << std::endl;

    MyClass objToModify;
    objToModify.setId(retrievedObj.getId());
    objToModify.setValue(20);
    MyClass& modifiedObj = mgr.Mod(objToModify);
    std::cout << "修改对象后，id: " << modifiedObj.getId() << ", 值: " << modifiedObj.getValue() << std::endl;

    if (mgr.Del(idx)) {
        std::cout << "删除对象成功" << std::endl;
    }

    return 0;
}
    