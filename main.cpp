#include <cmath>    
#include <iostream> 
#include <tuple>
#include <fstream>
#include <nlohmann/json.hpp>
#include "my_parser.hpp"

using namespace mup;

// Define global constants

constexpr float mu = 0.2;
constexpr float eta = 0.9;

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
    HeavyBall
};

// Choose your decay type

constexpr DecayType decay = DecayType::HeavyBall;

std::vector<double> operator*(double, const std::vector<double>& );
std::vector<double> operator-(const std::vector<double>&, const std::vector<double>&);
std::vector<double> operator+(const std::vector<double>&, const std::vector<double>&);

// Define the function to handle the decay

template<DecayType decayType>
float handleDecay(const Parameters &, int, my_Parser &, const std::vector<double> &);

double distance(const std::vector<double>&, const std::vector<double>&);

void readFunctionAndGradient(std::vector<double>&, my_Parser &, const Parameters &);

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

    if (sol.converged) 
    {
        std::cout << "The minimum is at: (";
        for (int i = 0; i < parameters.dim; ++i) 
        {
            std::cout << sol.minimum_coords[i];
            if (i != parameters.dim - 1) 
            {
                std::cout << ", ";
            }
        }
        std::cout << ")" << std::endl;
    }
    else 
    {
        std::cout << "The algorithm did not converge" << std::endl;
    }

    return 0;
}

template<DecayType decayType>
float handleDecay(const Parameters & parameters, int k, my_Parser & parser, const std::vector<double> & grad_evals)
{
    float alpha;
    if constexpr (decayType == DecayType::Exponential || decayType == DecayType::HeavyBall) 
    {
        // Handle Exponential decay of parameter alpha
        return parameters.a0 * std::exp(-mu * k);

    } else if constexpr (decayType == DecayType::Inverse) 
    {
        // Handle Inverse decay
        return parameters.a0/(1 + mu * k);

    } else if constexpr (decayType == DecayType::Armijo) 
    {
        // Handle Armijo decay
        float a = parameters.a0;
        std::vector<double> x0 = parser.getValues();
        double fun_x0_value = parser.evaluateFunction(x0);
        double grad_norm_x0_squared = std::inner_product(grad_evals.begin(), grad_evals.end(), grad_evals.begin(), 0.0);

        while (fun_x0_value - parser.evaluateFunction(x0 - (a*grad_evals)) < parameters.sigma * a * grad_norm_x0_squared) 
        {
            a /= 2;
        }

        return a;
    }
    else 
    {
        std::cout << "Invalid decay type" << std::endl;
        exit(1);
    }
}

template<DecayType decayType>
Solution ComputeMinimum(const Parameters& parameters)
{
    my_Parser parser(parameters.dim);

    std::vector<double> x0;

    readFunctionAndGradient(x0, parser, parameters);

    // Set the values of the variables
    
    parser.setValues(x0);

    //parser.printParser();

    // Define the parameters of the algorithm
    
    int k = 0;
    std::vector<double> d0(parameters.dim, 0.0);

    if constexpr (decayType == DecayType::HeavyBall)
    {
        d0 = (-parameters.a0) * parser.evaluateGradientFunction(x0);
        std::cout << "d0: " << d0[0] << " " << d0[1] << std::endl;
    }
    while(k < parameters.iter){

        std::vector<double> x1(parameters.dim);

        // Step 1: Set x0 to be the right value and set the gradient of the function at x0

        for (int i = 0; i < parameters.dim; ++i) 
            {
                x0[i] = parser.getValues()[i]; 
            }

        std::vector<double> gradientValues_x0(parser.evaluateGradientFunction(x0));

        // Step 2: Handle the decay of the parameter alpha

        float alpha = handleDecay<decayType>(parameters, k, parser, gradientValues_x0);

        // Step 3: Update the variable x1

        if constexpr (decayType == DecayType::Exponential || decayType == DecayType::Inverse || decayType == DecayType::Armijo)
        {
            x1 = x0 - alpha * gradientValues_x0;
        }
        else 
        {
            x1 = x0 + d0;
            d0 = eta * d0 - alpha * parser.evaluateGradientFunction(x1);
        }

        // Define the norm of the gradient

        double grad_norm_x0 = std::sqrt(std::inner_product(gradientValues_x0.begin(), gradientValues_x0.end(), gradientValues_x0.begin(), 0.0));

        // Step 4: Check the stopping criteria

        if (distance(x1, x0) < parameters.tol_r || grad_norm_x0 < parameters.tol_s) 
        {
            break;
        }

        //parser.printParser();

        // Print the iteration information

        //std::cout << "Iteration: " << k << "  ";
        //std::cout << "Distance: " << distance(x1, x0) << " Gradient norm: " << grad_norm_x0 << " Difference: " << 10 * distance(x1, x0) - grad_norm_x0 << std::endl;

        // Step 5: Update the parameters

        k++;

        // Step 6: Update the values

        parser.setValues(x1);

    }
    
    Solution solution;

    if (k == parameters.iter) 
    {
        std::cout << "The algorithm did not converge" << std::endl;
    }
    else
    {
        std::cout << "The algorithm converged in " << k << " iterations with value: " << parser.evaluateFunction(x0) << std::endl;
        
        solution.converged = true;

        for (int i = 0; i < parameters.dim; ++i) 
        {
            solution.minimum_coords.push_back(x0[i]);
        }
    }
    return solution;
}


void readFunctionAndGradient(std::vector<double>& initialConditions, my_Parser & parser, const Parameters& parameters) 
{
    // Open the file

    std::ifstream file_fun("function.txt");
    
    if (!file_fun) {
        std::cerr << "Unable to open file function.txt";
        exit(1);   // call system to stop
    }

    std::cout << "Function file opened successfully" << std::endl;

    // Read the initial conditions from the file

    std::string line;
    double value;
    std::getline(file_fun, line); // Skip the "//initial conditions" line
    std::getline(file_fun, line); // Read the initial conditions
    std::istringstream iss(line);

    while (iss >> value) 
    {
        initialConditions.push_back(value);
    }

    // Read the function from the file

    std::getline(file_fun, line); // Skip the "//function" line
    std::getline(file_fun, line); // Read the function
    parser.setFunction(line);

    // Read the gradient from the file

    if (parameters.gradient == "y") 
    {
        std::cout << "You want to define the gradient by yourself: "<< std::endl;
        std::getline(file_fun, line); // Skip the "//gradient" line
        for (int i = 0; i < parameters.dim; ++i) 
        {
            std::getline(file_fun, line); // Read a gradient function
            parser.setGradientFunction(i, line);
        }
    }
    else
    {
       std::cout << "We will provide a gradient for you..." << std::endl;
    }

    // Close the file
    file_fun.close();
}

double distance(const std::vector<double>& x1, const std::vector<double>& x0) 
{
    double sum = 0.0;
    for (std::size_t i = 0; i < x1.size(); ++i) {
        double diff = x1[i] - x0[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

std::vector<double> operator*(double a, const std::vector<double>& v) 
{
    std::vector<double> result(v.size());
    for (std::size_t i = 0; i < v.size(); ++i) {
        result[i] = v[i] * a;
    }
    return result;
}

std::vector<double> operator-(const std::vector<double>& v1, const std::vector<double>& v2) 
{
    std::vector<double> result(v1.size());
    for (std::size_t i = 0; i < v1.size(); ++i) {
        result[i] = v1[i] - v2[i];
    }
    return result;
}

std::vector<double> operator+(const std::vector<double>& v1, const std::vector<double>& v2) 
{
    std::vector<double> result(v1.size());
    for (std::size_t i = 0; i < v1.size(); ++i) {
        result[i] = v1[i] + v2[i];
    }
    return result;
}