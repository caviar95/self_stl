#include "user.hpp"
#include "object_manager.hpp"

int main() {
    ObjectManager<User> userManager;

    // 创建对象
    userManager.add(1, "Alice");
    userManager.add(2, "Bob");

    // 遍历打印
    userManager.forEach([](const User& user) {
        user.print();
    });

    // 更新对象
    userManager.update(0, 1, "Alex"); // 将第一个用户改为 Alex

    // 删除对象
    userManager.remove(1); // 删除第二个用户

    // 清空所有对象
    userManager.clear();

    return 0;
}