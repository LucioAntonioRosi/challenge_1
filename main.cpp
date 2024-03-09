#include <cmath>    // (for sqrt)
#include <iostream> // input output
#include <tuple>
#include <fstream>
#include <vector>
#include "nlohmann/json.hpp"
#include "mpParser.h"

using namespace mup;

// #include <vector>
// #include <cmath>

// Function to compute the minimum of a function

double computeMinimum()
{
   // Create a parser
ParserX parser(pckALL_NON_COMPLEX);
std::cout << "Parser created" << std::endl;
// Define the values
Value xVal(1.1), yVal(2.0);

// Define the variables

Variable x(&xVal), y(&yVal) ; 

 parser.DefineVar("x", x);
 parser.DefineVar("y", y);

// Get the function from the user

std::string function;
std::cout << "Enter a function in x and y: ";
std::getline(std::cin, function);

// Set the function
parser.SetExpr(function);

 // Placeholder return value
    Value result = parser.Eval();
    std::cout << "Result: " << result << std::endl;
    return 0;
}

int main()
{
    // Call the computeMinimum function
    double minimum = computeMinimum();

    // Print the minimum value
    std::cout << "Minimum value: " << minimum << std::endl;

     std::ifstream file("parameters.json");

    // Parse the JSON file
    nlohmann::json j;
    file >> j;

    // Get the parameters
    double a0 = j["a0"];
    std::string decay_string1 = j["decay"]["string1"];
    std::string decay_string2 = j["decay"]["string2"];
    std::string decay_string3 = j["decay"]["string3"];
    double sigma = j["sigma"];
    int iter = j["iter"];

    // Print the parameters
    std::cout << "a0: " << a0 << std::endl;
    std::cout << "decay_string1: " << decay_string1 << std::endl;
    std::cout << "decay_string2: " << decay_string2 << std::endl;
    std::cout << "decay_string3: " << decay_string3 << std::endl;
    std::cout << "sigma: " << sigma << std::endl;
    std::cout << "iter: " << iter << std::endl;
    return 0;
}