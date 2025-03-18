#include <iostream>
#include <memory>

// 组件接口
class Text {
public:
    virtual std::string render() = 0;
    virtual ~Text() = default;
};

// 具体组件
class PlainText : public Text {
public:
    std::string render() override {
        return "Plain Text";
    }
};

// 装饰器基类
class TextDecorator : public Text {
protected:
    std::unique_ptr<Text> _text;
public:
    TextDecorator(std::unique_ptr<Text> text) : _text(std::move(text)) {}
};

// 具体装饰器
class BoldDecorator : public TextDecorator {
public:
    using TextDecorator::TextDecorator;
    std::string render() override {
        return "<b>" + _text->render() + "</b>";
    }
};

class ItalicDecorator : public TextDecorator {
public:
    using TextDecorator::TextDecorator;
    std::string render() override {
        return "<i>" + _text->render() + "</i>";
    }
};

int main() {
    auto text = std::make_unique<PlainText>();
    auto boldText = std::make_unique<BoldDecorator>(std::move(text));
    auto italicBoldText = std::make_unique<ItalicDecorator>(std::move(boldText));

    std::cout << italicBoldText->render() << std::endl; // 输出 <i><b>Plain Text</b></i>
    return 0;
}