class Command {
public:
    virtual void execute() = 0;
};

class Light {
public:
    void on() { std::cout << "Light ON" << std::endl; }
};

class LightOnCommand : public Command {
private:
    Light _light;
public:
    void execute() override { _light.on(); }
};

class RemoteControl {
private:
    Command* _command;
public:
    void setCommand(Command* cmd) { _command = cmd; }
    void pressButton() { _command->execute(); }
};