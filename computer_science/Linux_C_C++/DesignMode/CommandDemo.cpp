#include <iostream>
#include <string>

//命令接口
class Command {
public:
    virtual ~Command() {}
    virtual void execute() = 0;
};

//具体命令
class ConcreteCommand : public Command {
public:
    ConcreteCommand(const std::string& message): message(message) {}

    void execute() override {
        std::cout << "Executing command: " << message << std::endl;
        //执行命令的具体操作
    }
private:
    std::string message;
};

//请求者
class Invoker {
public:
    void setCommand(Command* command) {
        this->command = command;
    }

    void executeCommand() {
        command->execute();
    }
private:
    Command* command;
};

//接收者
class Receiver {
public:
    void performAction() {
        std::cout << "Performing action" << std::endl;
        //执行接收者的操作
    }
};

int main(void) {
    Invoker invoker;
    Receiver receiver;
    Command* command = new ConcreteCommand("doing something");

    //设置命令
    invoker.setCommand(command);

    //执行命令
    invoker.executeCommand();

    //执行接收者的操作
    receiver.performAction();

    delete command;

    return 0;
}