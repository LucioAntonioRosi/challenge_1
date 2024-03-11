#ifndef MY_PARSER_HPP
#define MY_PARSER_HPP

// Include any necessary headers here

#include "mpParser.h"
#include <vector>
#include <functional>

// Declare any global constants or macros here

// Declare any classes or structs here
class my_Parser 
{
private:
    mup::ParserX fun;
    std::vector<mup::ParserX> grad;
    std::vector<mup::Value> values;
    std::vector<mup::Variable> variables;

public:
    my_Parser(int n);
    my_Parser() = delete; // No default constructor
    void setFunction(const std::string& expression);
    void setGradientFunction(int index, const std::string& expression);
    void setValues(const std::vector<double>& args);
    std::vector<double> getValues();
    double evaluateFunction();
    double evaluateFunction(const std::vector<double>& args); 
    std::vector<double> evaluateGradientFunction(const std::vector<double>& args);
    std::vector<double> evaluateGradientFunction();
    void printParser(); 
};


#endif // MY_PARSER_HPP