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

    void setFunction(const std::string& expression)
    {
        fun.SetExpr(expression);
    }

    void setGradientFunction(int index, const std::string& expression)
     {
        if (index >= 0 && index < grad.size()) {
            grad[index].SetExpr(expression);
        } else {
            throw std::out_of_range("Index out of range");
        }
    }

    void setValues(const std::vector<double>& args) 
    {
        for (int i = 0; i < args.size(); ++i) {
            values[i] = args[i];
        }
    }



    std::vector<double> getValues() 
    {
        std::vector<double> result;
        result.reserve(values.size());
        for (const auto& value : values) {
            result.push_back(value.GetFloat());
        }
        return result;
    }



    double evaluateFunction()
    {
        return fun.Eval().GetFloat();
    }



    double evaluateFunction(const std::vector<double>& args) 
    {
        setValues(args);
        return fun.Eval().GetFloat();
    }



    std::vector<double> evaluateGradientFunction(const std::vector<double>& args) 
    {
        setValues(args);
        std::vector<double> result;
        result.reserve(grad.size());
        for (const auto& parser : grad) 
        {
            result.push_back(parser.Eval().GetFloat());
        }
        return result;
    }
    


    std::vector<double> evaluateGradientFunction() 
    {
        std::vector<double> result;
        result.reserve(grad.size());
        for (const auto& parser : grad) 
        {
            result.push_back(parser.Eval().GetFloat());
        }
        return result;
    }