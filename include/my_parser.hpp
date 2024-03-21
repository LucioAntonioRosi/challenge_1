/* THIS HEADER FILE IS USED TO BETTER ORGANIZE 
THE CODE, BY HAVING A CLASS THAT HANDLES THE 
PARSING OF THE FUNCTION AND ITS GRADIENT.*/

#ifndef MY_PARSER_HPP
#define MY_PARSER_HPP

#include "mpParser.h"
#include <vector>
#include <functional>

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

    std::vector<double> getValues() const;

    double evaluateFunction() const;

    double evaluateFunction(const std::vector<double>& args); 

    double evaluatePartialDerDC(const std::vector<double>& args, int index, const float);

    std::vector<double> evaluateGradientDC(const std::vector<double>& args, const float h = 1e-6);

    std::vector<double> evaluateGradientFunction(const std::vector<double>& args);

    std::vector<double> evaluateGradientFunction() const;

    void printParser() const; 
};


#endif // MY_PARSER_HPP