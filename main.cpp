#include <cmath>    // (for sqrt)
#include <iostream> // input output
#include <tuple>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include "mpParser.h"
//#include "minimum.h"

using namespace mup;

// Define the struct
struct Parameters {
    double a0;
    int dim;
    std::vector<std::string> types;
    std::string gradient;
    double sigma;
    int iter;
};

int main()
{
    std::ifstream file("parameters.json");

    // Parse the JSON file
    
    nlohmann::json j;
    file >> j;

// Read the parameters from the JSON file
Parameters parameters;
parameters.a0 = j["a0"];
parameters.dim = j["dim"];
parameters.types.push_back(j["decay"]["type1"]);
parameters.types.push_back(j["decay"]["type2"]);
parameters.types.push_back(j["decay"]["type3"]);
parameters.gradient = j["gradient"];
parameters.sigma = j["sigma"];
parameters.iter = j["iter"];

    
    // Get the parameters from the JSON file
    
    // const std::tuple <double, int, std::string, std::string, std::string, std::string, double, int> &parameters = std::make_tuple(
    // j["a0"],j["dim"], j["decay"]["string1"], j["decay"]["string2"], j["decay"]["string3"],j["gradient"], j["sigma"], j["iter"]);


    // Create a parser
    ParserX parser(pckALL_NON_COMPLEX);
    std::vector<ParserX> parserVec;
    parserVec.reserve(parameters.dim);
    
    // Define the values and the variables
    std::vector<mup::Value> values;
    values.resize(parameters.dim,1.0);
    std::vector<mup::Variable> variables;
    
    // Define the variables in parser
    
    for (int i = 0; i < parameters.dim; ++i) {

    // Add the variables to the vectors
    variables.push_back(&values[i]);

    // Define the variable in the parser with a dynamically generated name
    std::string varName = "x" + std::to_string(i);
    parser.DefineVar(varName, variables[i]);
    std::cout << "Variable defined: " <<varName << std::endl;
}

    for (int i = 0; i < parameters.dim; ++i) {
        parserVec.push_back(parser);
    }
    
    std::string function;
    std::cout << "Enter a function in " << parameters.dim << " variables (x0, x1, x2, ...): ";
    std::getline(std::cin, function);
    parser.SetExpr(function);

    // Define the gradient
    if (parameters.gradient == "y")
    {
        std::cout << "You want to define the gradient by yourself: "<< std::endl;
        for (std::size_t i = 0; i < parameters.dim; ++i)
        {
            std::cout << "Enter a function in " << parameters.dim << "  variables (x0, x1, x2, ...): ";
            std::getline(std::cin, function);
            parserVec[i].SetExpr(function);
        }
        std::cout << "Gradient defined..." << std::endl;
    }
    else
    {
        std::cout << "We will provide a gradient for you..." << std::endl;
    }


    // Define the enum
enum DecayType 
{
    Exponential,
    Inverse,
    Armijo,
    Unknown
};

// Convert the decay string to a DecayType
std::string decayString;
DecayType decayType;

std::cout << "Enter the decay type (Exponential, Inverse, Armijo): ";
std::cin >> decayString;

if (decayString == "Exponential") 
{
    decayType = Exponential;
} else if (decayString == "Inverse") 
{
    decayType = Inverse;
} else if (decayString == "Armijo") 
{
    decayType = Armijo;
} else 
{
    decayType = Unknown;
}

// Handle the decay type with a switch statement
switch (decayType) {
    case Exponential:
        // Handle exponential
        break;
    case Inverse:
        // Handle inverse
        break;
    case Armijo:
        // Handle Armijo
        break;
    default:
        // Handle unknown decay type
        std::cout << "Unknown decay type: " << decayString << std::endl;
     break;
}

    return 0;
}






