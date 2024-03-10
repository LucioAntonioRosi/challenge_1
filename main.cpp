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

// Define the enum
enum DecayType 
{
    Exponential,
    Inverse,
    Armijo,
    Unknown
};

template<DecayType decayType>
void handleDecay();

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

for (int i = 0; i < parameters.dim; ++i) 
{
    parserVec.push_back(parser);
}

// Open the file
std::ifstream file_fun("function.txt");
if (!file_fun) {
    std::cerr << "Unable to open file function.txt";
    exit(1);   // call system to stop
}

// Read the function from the file
std::string line;
std::getline(file_fun, line); // Skip the "//function" line
std::getline(file_fun, line); // Read the function
parser.SetExpr(line);

// Read the gradient from the file
if (parameters.gradient == "y") 
{
    std::cout << "You want to define the gradient by yourself: "<< std::endl;
    std::getline(file_fun, line); // Skip the "//gradient" line
    for (int i = 0; i < parameters.dim; ++i) 
    {
        std::getline(file_fun, line); // Read a gradient function
        parserVec[i].SetExpr(line);
    }
}
else
{
   std::cout << "We will provide a gradient for you..." << std::endl;
}

// Evaluate the function
mup::Value functionValue = parser.Eval();
std::cout << "Function value: " << functionValue.GetFloat() << std::endl;

// Evaluate the gradient
std::vector<double> gradientValues(parameters.dim);
for (int i = 0; i < parameters.dim; ++i) {
    mup::Value gradientValue = parserVec[i].Eval();
    gradientValues[i] = gradientValue.GetFloat();
    std::cout << "Gradient value " << i << ": " << gradientValues[i] << std::endl;
}

// Close the file
file.close();
    
    // std::string function;
    // std::cout << "Enter a function in " << parameters.dim << " variables (x0, x1, x2, ...): ";
    // std::getline(std::cin, function);
    // parser.SetExpr(function);

    // Define the gradient

    // if (parameters.gradient == "y")
    // {
    //     std::cout << "You want to define the gradient by yourself: "<< std::endl;
    //     for (std::size_t i = 0; i < parameters.dim; ++i)
    //     {
    //         std::cout << "Enter a function in " << parameters.dim << "  variables (x0, x1, x2, ...): ";
    //         std::getline(std::cin, function);
    //         parserVec[i].SetExpr(function);
    //     }
    //     std::cout << "Gradient defined..." << std::endl;
    // }
    // else
    // {
    //     std::cout << "We will provide a gradient for you..." << std::endl;
    // }



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


template<DecayType decayType>
void handleDecay() {
    if constexpr (decayType == DecayType::Exponential) {
        std::cout << "Handling Exponential decay" << std::endl;
        // Handle Exponential decay
    } else if constexpr (decayType == DecayType::Inverse) {
        std::cout << "Handling Inverse decay" << std::endl;
        // Handle Inverse decay
    } else if constexpr (decayType == DecayType::Armijo) {
        std::cout << "Handling Armijo decay" << std::endl;
        // Handle Armijo decay
    } else {
        std::cout << "Unknown decay type" << std::endl;
        // Handle unknown decay type
    }
}




