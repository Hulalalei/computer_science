#include <iostream>
#include <unordered_map>

//上下文类
class Context {
public:
    bool getVariable(const std::string& name) {
        return variables[name];
    }
    void setVariable(const std::string& name, bool value) {
        variables[name] = value;
    }
private:
    std::unordered_map<std::string, bool> variables;
};

//抽象表达式类
class Expression {
public:
    virtual bool interpret(Context& context) = 0;
};

//终结符表达式类
class TerminalExpression : public Expression {
public:
    TerminalExpression(const std::string& variable): variable(variable) {}
    
    bool interpret(Context& context) override {
        return context.getVariable(variable);
    }
private:
    std::string variable;
};

//非终结符表达式类
class AndExpression : public Expression {
public:
    AndExpression(Expression* expression1, Expression* expression2): 
        expression1(expression1), expression2(expression2) {}

    bool interpret(Context& context) override {
        return expression1->interpret(context) && expression2->interpret(context);
    }
private:
    Expression* expression1;
    Expression* expression2;
};

//非终结符表达式类
class OrExpression : public Expression {
public:
    OrExpression(Expression* expression1, Expression* expression2): 
        expression1(expression1), expression2(expression2) {}

    bool interpret(Context& context) override {
        return expression1->interpret(context) || expression2->interpret(context);
    }
private:
    Expression* expression1;
    Expression* expression2;
};

int main(void) {
    Context context;
    context.setVariable("A", true);
    context.setVariable("B", true);

    Expression* expression1 = new TerminalExpression("A");
    Expression* expression2 = new TerminalExpression("B");

    //A AND B
    Expression* andExpression = new AndExpression(expression1, expression2);
    bool result = andExpression->interpret(context);
    std::cout << "A AND B " << result << std::endl;

    //A OR B
    Expression* orExpression = new OrExpression(expression1, expression2);
    result = orExpression->interpret(context);
    std::cout << "A OR B " << result << std::endl;

    delete expression1;
    delete expression2;
    delete andExpression;
    delete orExpression;

    return 0;
}