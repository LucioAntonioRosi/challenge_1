#include "my_parser.hpp"

my_Parser::my_Parser(int n) : fun(mup::pckALL_NON_COMPLEX), grad(n, mup::ParserX(mup::pckALL_NON_COMPLEX)),values(n, mup::Value(0.0))
{
    for (int i = 0; i < n; ++i) 
    {
        variables.push_back(&values[i]);
    }
    // Define n variables for fun and each ParserX in grad
    for (int i = 0; i < n; ++i) 
    {
        std::string varName = "x" + std::to_string(i);
        fun.DefineVar(varName, variables[i]);
        for (auto& parser : grad) 
        {
            parser.DefineVar(varName, variables[i]);
        }
    }
}

void my_Parser::setFunction(const std::string& expression)
{
    fun.SetExpr(expression);
}

void my_Parser::setGradientFunction(int index, const std::string& expression)
{
    int n = grad.size();
    if (index >= 0 && index < n) {
        grad[index].SetExpr(expression);
    } else {
        throw std::out_of_range("Index out of range");
    }
}

void my_Parser::setValues(const std::vector<double>& args) 
{
    int n = args.size();
        for (int i = 0; i < n; ++i) 
        {
            values[i] = args[i];
        }
}

std::vector<double> my_Parser::getValues() const 
{
    std::vector<double> result;
    result.reserve(values.size());
    for (const auto& value : values) {
        result.push_back(value.GetFloat());
    }
    return result;
}



double my_Parser::evaluateFunction() const
{
    return fun.Eval().GetFloat();
}



double my_Parser::evaluateFunction(const std::vector<double>& args) 
{
    setValues(args);
    return evaluateFunction();
}

std::vector<double> my_Parser::evaluateGradientFunction() const 
{
    std::vector<double> result;
    result.reserve(grad.size());
    for (const auto& parser : grad) 
    {
        result.push_back(parser.Eval().GetFloat());
    }
    return result;
}

std::vector<double> my_Parser::evaluateGradientFunction(const std::vector<double>& args) 
{
    setValues(args);
    return evaluateGradientFunction();
}

double my_Parser::evaluatePartialDerDC(const std::vector<double>& args, int index, const float h)
{
    std::vector<double> args_pos(args),args_neg(args);
    args_pos[index] += h;
    args_neg[index] -= h;
    double result(1/(2*h)*(evaluateFunction(args_pos) - evaluateFunction(args_neg)));
    setValues(args);
    return result; 
}
    
std::vector<double> my_Parser::evaluateGradientDC(const std::vector<double>& args, const float h) 
{
    std::vector<double> result;
    result.reserve(grad.size());
    int n = args.size();
    for (int i = 0; i < n; ++i) 
    {
        result.push_back(evaluatePartialDerDC(args, i, h));
    }
    return result;
}

    void my_Parser::printParser() const 
{
    // Print the vector of values
    
    std::cout << "Current values:\n";
    std::cout << "-------\n";
    for (const auto& value : values) {
        std::cout << value.GetFloat() << '\n';
    }
    std::cout << "-------\n\n";

    // Print the function expression
   
     std::cout << "Function Expression:\n";
    std::cout << "-------------------\n";
    std::cout << fun.GetExpr() << '\n';
    std::cout << "-------------------\n\n";

    // Print the gradient expressions
    std::cout << "Gradient Expressions:\n";
    std::cout << "---------------------\n";
    for (size_t i = 0; i < grad.size(); ++i) {
        std::cout << "Gradient " << (i + 1) << ": " << grad[i].GetExpr() << '\n';
    }
    std::cout << "---------------------\n\n";
}   