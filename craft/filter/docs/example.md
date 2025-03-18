以下是一个基于学校、年级、班级和性别进行筛选的C++实现，包含完整代码和详细注释：

```cpp
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>

// 学生性别枚举
enum class Gender {
    Male,
    Female,
    Other
};

// 学生数据类
class Student {
public:
    Student(const std::string& school, 
           int grade, 
           int class_num,
           Gender gender)
        : school_(school), 
          grade_(grade),
          class_num_(class_num),
          gender_(gender) {}

    // 访问器方法
    const std::string& school() const { return school_; }
    int grade() const { return grade_; }
    int class_num() const { return class_num_; }
    Gender gender() const { return gender_; }

private:
    std::string school_;
    int grade_;
    int class_num_;
    Gender gender_;
};

// 过滤器基类
class Filter {
public:
    virtual ~Filter() = default;
    virtual bool match(const Student& student) const = 0;
};

// 学校过滤器（精确匹配）
class SchoolFilter : public Filter {
public:
    explicit SchoolFilter(const std::string& school) 
        : target_school_(school) {}

    bool match(const Student& student) const override {
        return student.school() == target_school_;
    }

private:
    std::string target_school_;
};

// 年级过滤器（精确匹配）
class GradeFilter : public Filter {
public:
    explicit GradeFilter(int grade) 
        : target_grade_(grade) {}

    bool match(const Student& student) const override {
        return student.grade() == target_grade_;
    }

private:
    int target_grade_;
};

// 班级过滤器（精确匹配）
class ClassFilter : public Filter {
public:
    explicit ClassFilter(int class_num)
        : target_class_(class_num) {}

    bool match(const Student& student) const override {
        return student.class_num() == target_class_;
    }

private:
    int target_class_;
};

// 性别过滤器（精确匹配）
class GenderFilter : public Filter {
public:
    explicit GenderFilter(Gender gender)
        : target_gender_(gender) {}

    bool match(const Student& student) const override {
        return student.gender() == target_gender_;
    }

private:
    Gender target_gender_;
};

// 组合过滤器（逻辑与）
class AndFilter : public Filter {
public:
    void add(std::unique_ptr<Filter> filter) {
        filters_.push_back(std::move(filter));
    }

    bool match(const Student& student) const override {
        for (const auto& filter : filters_) {
            if (!filter->match(student)) return false;
        }
        return true;
    }

private:
    std::vector<std::unique_ptr<Filter>> filters_;
};

// 学生数据库类
class StudentDatabase {
public:
    void add_student(Student student) {
        students_.push_back(std::move(student));
    }

    std::vector<Student> query(const Filter& filter) const {
        std::vector<Student> results;
        for (const auto& student : students_) {
            if (filter.match(student)) {
                results.push_back(student);
            }
        }
        return results;
    }

private:
    std::vector<Student> students_;
};

int main() {
    StudentDatabase db;

    // 添加测试数据
    db.add_student(Student("第一中学", 3, 2, Gender::Female));
    db.add_student(Student("第一中学", 3, 2, Gender::Male));
    db.add_student(Student("第二中学", 3, 2, Gender::Female));
    db.add_student(Student("第一中学", 4, 2, Gender::Female));

    // 构建组合过滤器
    AndFilter condition;
    condition.add(std::make_unique<SchoolFilter>("第一中学"));
    condition.add(std::make_unique<GradeFilter>(3));
    condition.add(std::make_unique<ClassFilter>(2));
    condition.add(std::make_unique<GenderFilter>(Gender::Female));

    // 执行查询
    auto results = db.query(condition);

    // 输出结果
    std::cout << "符合条件的学生数量: " << results.size() << "\n";
    for (const auto& student : results) {
        std::cout << "学校: " << student.school()
                  << " 年级: " << student.grade()
                  << " 班级: " << student.class_num()
                  << " 性别: " 
                  << (student.gender() == Gender::Female ? "女" : 
                     (student.gender() == Gender::Male ? "男" : "其他"))
                  << "\n";
    }

    return 0;
}
```

### 设计说明

1. **数据结构**：
   - `Student` 类包含学校、年级、班级和性别字段
   - 使用枚举类型 `Gender` 表示性别

2. **过滤器系统**：
   - **基础过滤器**：实现精确匹配的学校、年级、班级、性别过滤器
   - **组合过滤器**：通过 `AndFilter` 实现逻辑与组合
   - **扩展性**：可轻松添加`OrFilter`或`RangeFilter`等新过滤器

3. **数据库类**：
   - `StudentDatabase` 负责数据存储和查询
   - `query` 方法接收过滤器对象并返回匹配结果

4. **使用示例**：
   - 组合多个条件进行精确查询
   - 显示格式化的查询结果

### 执行结果

```
符合条件的学生数量: 1
学校: 第一中学 年级: 3 班级: 2 性别: 女
```

### 扩展建议

1. **添加范围查询**：
```cpp
class GradeRangeFilter : public Filter {
public:
    GradeRangeFilter(int min, int max) 
        : min_grade_(min), max_grade_(max) {}
    
    bool match(const Student& s) const override {
        return s.grade() >= min_grade_ 
            && s.grade() <= max_grade_;
    }
private:
    int min_grade_;
    int max_grade_;
};
```

2. **添加逻辑或组合**：
```cpp
class OrFilter : public Filter {
public:
    // 实现与AndFilter类似的逻辑
    // 任意子过滤器匹配即返回true
};
```

3. **添加否定条件**：
```cpp
class NotFilter : public Filter {
public:
    explicit NotFilter(std::unique_ptr<Filter> filter)
        : filter_(std::move(filter)) {}
    
    bool match(const Student& s) const override {
        return !filter_->match(s);
    }
private:
    std::unique_ptr<Filter> filter_;
};
```

4. **支持模糊查询**：
```cpp
class FuzzySchoolFilter : public Filter {
public:
    explicit FuzzySchoolFilter(const std::string& keyword)
        : keyword_(keyword) {}
    
    bool match(const Student& s) const override {
        return s.school().find(keyword_) != std::string::npos;
    }
private:
    std::string keyword_;
};
```

该实现通过清晰的类层次结构和组合模式，提供了灵活的条件查询能力，可根据实际需求进行扩展和定制。