// DIP 高层模块不应该依赖底层模块，两者都应该依赖抽象，抽象不应该依赖细节，细节应该依赖抽象

// 错误示例
class Database {
public:
    void save() {
        // 保存数据到数据库
    }
};

class User {
    Database db; // User 依赖于 Database
public:
    void save() {
        db.save();
    }
};

// 改进示例
class IDatabase {
public:
    virtual void save() = 0; // 抽象类，定义了一个接口
};

class Database : public IDatabase {
public:
    void save() override {
        // 实现保存数据的具体逻辑
    }
};

class User {
protected:
    std::unique_ptr<IDatabase> db; // User 通过指针依赖抽象
public:
    User(std::unique_ptr<IDatabase> database) : db(move(database)) {}
    void save() {
        db->save();
    }
};

int main() {
    Database realDb;
    User user(&realDb);
    user.save();
    return 0;
}