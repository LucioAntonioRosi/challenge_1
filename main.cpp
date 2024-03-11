#include <cmath>    
#include <iostream> 
#include <tuple>
#include <fstream>
#include <nlohmann/json.hpp>
#include "my_parser.hpp"

using namespace mup;

// Define global constants

constexpr float mu = 0.2;

// Define the structs
struct Parameters 
{
    float a0;
    int dim;
    std::vector<std::string> types;
    std::string gradient;
    float sigma;
    float tol_r;
    float tol_s;
    int iter;
};

struct Solution
{
    bool converged = false;
    std::vector<double> minimum_coords;
    double minimum;
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

// Choose your decay type

constexpr DecayType decay = DecayType::Exponential;

// Define the function to handle the decay

template<DecayType decayType>
float handleDecay(float, float, int, std::vector<Value>&, const mup::ParserX&, std::vector<double>);

double distance(const std::vector<double>&, const std::vector<double>&);

void readFunctionAndGradient(std::vector<double>&, mup::ParserX&, std::vector<mup::ParserX>&, const Parameters&);

template<DecayType decayType>
Solution ComputeMinimum(const Parameters&);

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
    parameters.gradient = j["gradient"];
    parameters.sigma = j["sigma"];
    parameters.tol_r = j["tol_r"];
    parameters.tol_s = j["tol_s"];
    parameters.iter = j["iter"];

    Solution sol(ComputeMinimum<decay>(parameters));

    return 0;
}


template<DecayType decayType>
float handleDecay(float alpha0, float sigma, int k, std::vector<Value>& values, const mup::ParserX& parser, std::vector<double> grad_evals)
{
    float alpha;
    if constexpr (decayType == DecayType::Exponential) {
        return alpha0 * std::exp(-mu * k);
        std::cout << "alpha now has value: " << alpha<< std::endl;
        // Handle Exponential decay
    } else if constexpr (decayType == DecayType::Inverse) {
        return alpha0/(1 + mu * k);
        std::cout << "Handling Inverse decay" << std::endl;
        // Handle Inverse decay
    } else if constexpr (decayType == DecayType::Armijo) {
        
        std::cout << "Handling Armijo decay" << std::endl;
        // Handle Armijo decay
    } else {
        std::cout << "Unknown decay type" << std::endl;
        // Handle unknown decay type
        exit(1);
    }
}

template<DecayType decayType>
Solution ComputeMinimum(const Parameters& parameters)
{
    // Create a parser and a parser vector (we will use them to define the function and the gradient)

    // ParserX parser(pckALL_NON_COMPLEX);
    // std::vector<ParserX> parserVec;
    // parserVec.reserve(parameters.dim);

    // I am also defining a parserVec to store the gradient functions
    
    // ParserX parseraux(pckALL_NON_COMPLEX);
    
    // Define the values and the variables
    
    // std::vector<mup::Value> values;
    // values.resize(parameters.dim,0.0);
    // std::vector<mup::Variable> variables;
    // std::vector<mup::Value> valuesVec;
    // valuesVec.resize(parameters.dim,0.0);
    // std::vector<mup::Variable> variablesVec;
    
    // Define the variables in parser
    
    // for (int i = 0; i < parameters.dim; ++i) 
    // { 
    //     // Add the variables to the vectors

    //     variables.push_back(&values[i]);
    //     variablesVec.push_back(&valuesVec[i]);

    //     // Define the variables in the parser and parserVec

    //     std::string varName = "x" + std::to_string(i);
    //     parser.DefineVar(varName, variables[i]);
    //     parseraux.DefineVar(varName, variablesVec[i]);
    //     std::cout << "Variable defined: " <<varName << std::endl;
    // }

    // I DON'T KNOW WHY BUT IF I PUT PARSERVEC INSIDE THE FOR ABOVE IT DOES NOT WORK

    // for (int i = 0; i < parameters.dim; ++i)
    // {
    //     parserVec.push_back(parseraux);
    // }

    // Pass everything from the file

    my_Parser parser(parameters.dim);

    std::vector<double> x0;

    readFunctionAndGradient(x0, parser, parserVec, parameters);


std::cout << "\n\n\n";
    // Print x0
std::cout << "x0: ";
for (const auto& value : x0) {
    std::cout << value << ' ';
}
std::cout << '\n';

// Print the evaluation of parser
std::cout << "parser evaluation: " << parser.Eval().GetFloat() << '\n';


// Print the evaluations of parserVec
std::cout << "parserVec evaluations: ";
for (const auto& p : parserVec) {
    std::cout << p.Eval().GetFloat() << ' ';
    std::cout << "Expression: " << p.GetExpr() << std::endl;
}
std::cout << '\n';


    // Define the values of the variables
    
    for (int i = 0; i < parameters.dim; ++i) 
    {
        values[i] = x0[i];
        valuesVec[i] = x0[i];
    }

 // Print values
std::cout << "values: ";
for (const auto& value : values) {
    std::cout << value << ' ';
}
std::cout << '\n';

// Print valuesVec
std::cout << "valuesVec: ";
for (const auto& value : valuesVec) {
    std::cout << value.GetFloat() << ' ';
}
std::cout << '\n';
    

    // Define the parameters of the algorithm
    
    float alpha = parameters.a0;
    int k = 0;
    while(k < parameters.iter){

        std::vector<double> x1(parameters.dim);

        // Step 1: Evaluate the gradient at x0 and compute x1

        std::vector<double> gradientValues_x0(parameters.dim);

        for (int i = 0; i < parameters.dim; ++i) 
        {
            x0[i] = valuesVec[i];
            gradientValues_x0[i] = parserVec[i].Eval().GetFloat();
        }
        
        alpha = handleDecay<decayType>(parameters.a0,parameters.sigma, k, values, parser, gradientValues_x0);

        for (int i = 0; i < parameters.dim; ++i){
            x1[i] = valuesVec[i].GetFloat() - alpha * gradientValues_x0[i];
        }
        // Print gradientValues_x0
        //std::cout << "GradientValues: ";
        // for (const auto& value : gradientValues_x0) 
        // {
        //     std::cout << value << ' ';
        // }
        // std::cout << '\n';


         // Print values
        // std::cout << "values: ";
        // for (const auto& value : values) 
        // {
        //     std::cout << value << ' ';
        // }
        // std::cout << '\n';

        // Print valuesVec
        // std::cout << "valuesVec: ";
        // for (const auto& value : valuesVec) 
        // {
        //     std::cout << value.GetFloat() << ' ';
        // }
        // std::cout << '\n';

        if (distance(x1, x0) < parameters.tol_r ||
         std::sqrt(std::inner_product(gradientValues_x0.begin(), gradientValues_x0.end(), gradientValues_x0.begin(), 0.0)) < parameters.tol_s) 
        {
            break;
        }

        // Step 2: Update the parameters

        k++;

        // Step 3: Update the variables

        for (int i = 0; i < parameters.dim; ++i) 
        {
            values[i] = x1[i];
            valuesVec[i] = x1[i];
        }
    }
    
    Solution solution;

    if (k == parameters.iter) 
    {
        std::cout << "The algorithm did not converge" << std::endl;
    }
    else
    {
        std::cout << "The algorithm converged in " << k << " iterations with value: " <<parser.Eval().GetFloat() << std::endl;
        
        for (int i = 0; i < parameters.dim; ++i) 
        {
            solution.minimum_coords.push_back(x0[i]);
        }
    }
    return solution;
}


void readFunctionAndGradient(std::vector<double>& initialConditions, mup::ParserX& parser, std::vector<mup::ParserX>& parserVec, const Parameters& parameters) {
    // Open the file
    std::ifstream file_fun("function.txt");
    if (!file_fun) {
        std::cerr << "Unable to open file function.txt";
        exit(1);   // call system to stop
    }

    std::cout << "File opened successfully" << std::endl;

    // Read the initial conditions from the file

    std::string line;
    double value;
    std::getline(file_fun, line); // Skip the "//initial conditions" line
    std::cout << "Read line: " << line << std::endl;
    std::getline(file_fun, line);
    std::cout << "Read line: " << line << std::endl;

    std::istringstream iss(line);
    int i = 0;
    while (iss >> value) {
        initialConditions.push_back(value);
        std::cout << "Initial condition " << i << ": " << value << std::endl;
        i++;
    }

    // Read the function from the file

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

    // Close the file
    file_fun.close();
}

double distance(const std::vector<double>& x1, const std::vector<double>& x0) {
    double sum = 0.0;
    for (std::size_t i = 0; i < x1.size(); ++i) {
        double diff = x1[i] - x0[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}