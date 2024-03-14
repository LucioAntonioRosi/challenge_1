#include <cmath>    
#include <iostream> 
#include <fstream>
#include <nlohmann/json.hpp>
#include "my_parser.hpp"
#include "methods.hpp"

using namespace mup;

// Define the structs

struct Parameters 
{
    float a0;
    float eta;
    float mu;
    int dim;
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

// Define the operators

std::vector<double> operator*(double, const std::vector<double>&);
std::vector<double> operator-(const std::vector<double>&, const std::vector<double>&);
std::vector<double> operator+(const std::vector<double>&, const std::vector<double>&);

// Define the function to handle the decay

template<DecayType decayType, SecondOrderMethod method>
float handleDecay (const Parameters &, int, my_Parser &, const std::vector<double> &);

// Define the function to compute the minimum

template<DecayType decayType, SecondOrderMethod method>
Solution ComputeMinimum (const Parameters&);

// Other auxiliary functions

double distance(const std::vector<double>&, const std::vector<double>&);

void readFunctionAndGradient(std::vector<double>&, my_Parser &, const Parameters &);

void printSolution(const Solution&);

Parameters readParameters(const std::string&);

// Main function

int main()
{
    // Read the parameters from the JSON file

    Parameters parameters(readParameters("parameters.json"));

    // Compute the minimum
    
    Solution sol(ComputeMinimum<decay,method>(parameters));

    // Print the solution

    printSolution(sol);

    return 0;
}

template<DecayType decayType, SecondOrderMethod method>
float handleDecay(const Parameters & parameters, int k, my_Parser & parser, const std::vector<double> & grad_evals)
{
    if constexpr (decayType == DecayType::Exponential) 
    {
        return parameters.a0 * std::exp(-parameters.mu * k);

    } else if constexpr (decayType == DecayType::Inverse) 
    {
        return parameters.a0/(1 + parameters.mu * k);

    } else if constexpr (decayType == DecayType::Armijo && method == SecondOrderMethod::None) 
    {
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
        std::cout << "Wrong combination of decay type and second order method" << std::endl;
        exit(1);
    }
}

template<DecayType decayType, SecondOrderMethod method>
Solution ComputeMinimum (const Parameters& parameters)
{
    my_Parser parser(parameters.dim);

    std::vector<double> x0;

    readFunctionAndGradient(x0, parser, parameters);

    // Set the values of the variables
    
    parser.setValues(x0);

    parser.printParser();

    // Define the parameters of the algorithm
    
    int k = 0;
    std::vector<double> d0(parameters.dim, 0.0);
    std::vector<double> x1(parameters.dim);

    if constexpr (method == SecondOrderMethod::HeavyBall)
    {
        if constexpr (Numerical_grad == "Y")
            d0 = (-parameters.a0) * parser.evaluateGradientFunction(x0);
        else if constexpr(Numerical_grad == "N")
        d0 = (-parameters.a0) * parser.evaluateGradientDC(x0);
    }
    


    while (k < parameters.iter) {

        // Step 0: Define an aux variable for Nestorov method

        std::vector<double> aux(parameters.dim, 0.0); //This represents x(k-1) but I can't name it x-1 or xk-1

        // Step 1: Set x0 to be the right value and set the gradient of the function at x0

        for (int i = 0; i < parameters.dim; ++i) 
            {
                x0[i] = parser.getValues()[i]; 
            }

        std::vector<double> gradientValues_x0(parameters.dim);

        if constexpr (Numerical_grad == "Y")
            gradientValues_x0 = parser.evaluateGradientFunction(x0);
        else if constexpr(Numerical_grad == "N")
            gradientValues_x0 = parser.evaluateGradientDC(x0);

        // Step 2: Handle the decay of the parameter alpha and update the variable x1

        if constexpr (method == SecondOrderMethod::None)
        {
            float alpha = handleDecay<decayType,method>(parameters, k, parser, gradientValues_x0);

            x1 = x0 - alpha * gradientValues_x0;
        }

        else if constexpr (method == SecondOrderMethod::Nesterov)
        {
            float alpha = handleDecay<decayType,method>(parameters, k, parser, gradientValues_x0);
            
            static bool first = true;
            
            if(first)
            {
                x1 = x0 - alpha * gradientValues_x0;
                first = false;
            }
            else
            {
                std::vector<double> y(x0 - parameters.eta * (x0 - aux));

                if constexpr (Numerical_grad == "Y")
                    x1 = y - alpha * parser.evaluateGradientFunction(y);
                else if constexpr(Numerical_grad == "N")
                    x1 = y - alpha * parser.evaluateGradientDC(y); 
            }
        }

        else if constexpr (method == SecondOrderMethod::HeavyBall)
        {
            x1 = x0 + d0;
            float alphak1 = handleDecay<decayType,method>(parameters, k + 1, parser, gradientValues_x0);
            if constexpr (Numerical_grad == "Y")
                d0 = parameters.eta * d0 - alphak1 * parser.evaluateGradientFunction(x1);
            else if constexpr(Numerical_grad == "N")
                d0 = parameters.eta * d0 - alphak1 * parser.evaluateGradientDC(x1);
        }

        // Step 3: Compute the norm of the gradient at x0

        double grad_norm_x0 = std::sqrt(std::inner_product(gradientValues_x0.begin(), gradientValues_x0.end(), gradientValues_x0.begin(), 0.0));

        // Step 4: Update the parameters

        k++;

        // Step 5: Update the values

        parser.setValues(x1);

        // Step 6: Check the stopping criteria

        if (distance(x1, x0) < parameters.tol_r || grad_norm_x0 < parameters.tol_s) 
        {
            break;
        }
        
        // Step 7: Update the aux variable for Nestorov method

        if constexpr (method == SecondOrderMethod::Nesterov)
        {
            aux = x0;
        }
    }
    
    Solution solution;

    if (k < parameters.iter)
    { 
        solution.converged = true;
    }
        solution.iter = k;

        solution.minimum = parser.evaluateFunction();

        for (int i = 0; i < parameters.dim; ++i) 
        {
            solution.minimum_coords.push_back(x0[i]);
        }

    return solution;
}

Parameters readParameters(const std::string& filename)
{
    // Open the file

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Unable to open file json";
        exit(1);
    }

    // Parse the JSON file

    nlohmann::json j;
    file >> j;

    // Read the parameters from the JSON file

    Parameters parameters;
    parameters.a0 = j["a0"];
    parameters.eta = j["eta"];
    parameters.mu = j["mu"];
    parameters.dim = j["dim"];
    parameters.sigma = j["sigma"];
    parameters.tol_r = j["tol_r"];
    parameters.tol_s = j["tol_s"];
    parameters.iter = j["iter"];

    return parameters;
}


void readFunctionAndGradient(std::vector<double>& initialConditions, my_Parser & parser, const Parameters& parameters) 
{
    // Open the file

    std::ifstream file_fun("function.txt");
    
    if (!file_fun) {
        std::cerr << "Unable to open file function.txt";
        exit(1);   // call system to stop
    }

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

    if (Numerical_grad == "Y") 
    {
        std::cout << "The gradient has been defined from the function file...\n"<< std::endl;
        std::getline(file_fun, line); // Skip the "//gradient" line
        for (int i = 0; i < parameters.dim; ++i) 
        {
            std::getline(file_fun, line); // Read a gradient function
            parser.setGradientFunction(i, line);
        }
    }
    else if (Numerical_grad == "N")
    {
       std::cout << "We will provide a gradient for you...\n" << std::endl;
    }
    else
    {
        std::cout << "Wrong string used for Numerical_grad. " << std::endl;
        exit(1);
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

void printSolution(const Solution& sol)
{
    // Check if the solution has converged
    if (sol.converged) 
    {
        std::cout << "Solution Converged at iteration: " << sol.iter << "\n";
        std::cout << "-----------------------------------\n";

        // Print the decay type
        std::cout << "Decay Type: " << decay << "\n";

        // Print the method used
        std::cout << "Method Used: " << method << "\n";

        // Print the minimum coordinates
        std::cout << "The minimum is at: (";
        for (size_t i = 0; i < sol.minimum_coords.size(); ++i) 
        {
            std::cout << sol.minimum_coords[i];
            if (i != sol.minimum_coords.size() - 1) 
            {
                std::cout << ", ";
            }
        }
        std::cout << ")\n";

        // Print the minimum value
        std::cout << "With corresponding value at: " << sol.minimum << "\n";

        std::cout << "-----------------------------------\n";
    }
    else 
    {
        std::cout << "The algorithm did not converge.\n";

        // Print the decay type
        std::cout << "Decay Type: " << decay << "\n";

        // Print the method used
        std::cout << "Method Used: " << method << "\n";

        // Print the minimum coordinates
        std::cout << "The minimum found is at: (";
        for (size_t i = 0; i < sol.minimum_coords.size(); ++i) 
        {
            std::cout << sol.minimum_coords[i];
            if (i != sol.minimum_coords.size() - 1) 
            {
                std::cout << ", ";
            }
        }
        std::cout << ")\n";

        // Print the minimum value
        std::cout << "With corresponding value at: " << sol.minimum << "\n";

        std::cout << "-----------------------------------\n";


    }
}