#include <cmath>    // (for sqrt)
#include <iostream> // input output
#include <tuple>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include "mpParser.h"
//#include "minimum.h"

using namespace mup;

int main()
{
    std::ifstream file("parameters.json");

    // Parse the JSON file
    
    nlohmann::json j;
    file >> j;
    
    // Get the parameters from the JSON file
    
    const std::tuple <double, int, std::string, std::string, std::string, std::string, double, int> &parameters = std::make_tuple(
    j["a0"],j["dim"], j["decay"]["string1"], j["decay"]["string2"], j["decay"]["string3"],j["gradient"], j["sigma"], j["iter"]);

    // Print the parameters
    std::cout << "Parameters: " << std::get<0>(parameters) << ", " << std::get<1>(parameters) << ", " << std::get<2>(parameters) << ", " << std::get<3>(parameters) << ", " << std::get<4>(parameters) << ", " << std::get<5>(parameters) << std::endl;
    
    // Create a parser
    ParserX parser(pckALL_NON_COMPLEX);
    std::vector<ParserX> parserVec;
    parserVec.reserve(std::get<1>(parameters));
    std::cout << "Parser created and Parser vec created" << std::endl;
    
    // Define the values and the variables
    std::vector<mup::Value> values;
    std::vector<mup::Variable> variables;
    
    //mup::Value value(1.0); // Initialize the Value with 1.0
    //mup::Variable variable(&value);

    // Define the variables in parser
    
    for (int i = 0; i < 2; ++i) {

    mup::Value* value = new mup::Value(i + 1.0); // Initialize the Value with i + 1.0
    mup::Variable variable(value);
    // Add the Value and Variable to the vectors
    values.push_back(&value);
    variables.push_back(&variable);

    // Define the variable in the parser with a dynamically generated name
    std::string varName = "x" + std::to_string(i);
    parser.DefineVar(varName, variables[i]);
    std::cout << "Variable defined: " <<varName << std::endl;
}

    for (int i = 0; i < std::get<1>(parameters); ++i) {
        parserVec.push_back(parser);
    }

    std::cout << "Parser vec size: " << parserVec.size() << std::endl;
    
    value = 2.0;
    
    std::string function;
    std::cout << "Enter a function in " << 2 << "  variables (x0, x1, x2, ...): ";
    std::getline(std::cin, function);
    parser.SetExpr(function);

    Value result = parser.Eval();
    
    std::cout << "Result: " << result << std::endl;

    // Define the gradient

    std::cout << "Do you want to define the gradient by yourself? (y/n): "<< std::endl;
        for (std::size_t i = 0; i < 2; ++i)
        {
            std::string function;
            std::cout << "Enter a function in " << 2 << "  variables (x0, x1, x2, ...): ";

            std::getline(std::cin, function);
            try {
            parserVec[i].SetExpr(function);
        } catch (mup::ParserError& e) {
            std::cout << "Error: " << e.GetMsg() << std::endl;
        }
        }

        
    return 0;
    }





