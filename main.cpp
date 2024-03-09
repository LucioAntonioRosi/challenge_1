#include <iostream>
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

    return 0;
}