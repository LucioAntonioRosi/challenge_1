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
    float sigma;
    float beta1;
    float beta2;
    float epsilon;
    int dim;
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

    // Define the different variables for the algorithm
    
    int k = 0;
    std::vector<double> d0(parameters.dim);
    std::vector<double> x1(parameters.dim,0.0);
    std::vector<double> x_old(parameters.dim); //This represents x(k-1) but I can't name it x-1 or xk-1
    std::vector<double> gradientValues_x0(parameters.dim);
    std::vector<double> m(parameters.dim,0.0);
    std::vector<double> v(parameters.dim,0.0);

    if constexpr (method == SecondOrderMethod::HeavyBall)
    {
        if constexpr (DefineGrad == "Y")
            d0 = -parameters.a0 * parser.evaluateGradientFunction(x0);
        else if constexpr(DefineGrad == "N")
        d0 = -parameters.a0 * parser.evaluateGradientDC(x0);
    }
    
    


    while (k < parameters.iter) {

        // Step 1: Set x0 to be the right value and set the gradient of the function at x0

        if constexpr (DefineGrad == "Y")
            gradientValues_x0 = parser.evaluateGradientFunction(x0);
        else if constexpr(DefineGrad == "N")
            gradientValues_x0 = parser.evaluateGradientDC(x0);
        else 
        {
            std::cout << "Wrong choice of gradient definition" << std::endl;
            exit(1);
        }

        // Step 2: Handle the decay of the parameter alpha and update the variable x1 based on the method used

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
                std::vector<double> y(x0 + parameters.eta*(x0 - x_old));

                if constexpr (DefineGrad == "Y")
                    x1 = y - alpha * parser.evaluateGradientFunction(y);
                else if constexpr(DefineGrad == "N")
                    x1 = y - alpha * parser.evaluateGradientDC(y); 
            }
            x_old = x0;
        }

        else if constexpr (method == SecondOrderMethod::HeavyBall)
        {
            x1 = x0 + d0;
            float alphak1 = handleDecay<decayType,method>(parameters, k + 1, parser, gradientValues_x0);
            if constexpr (DefineGrad == "Y")
                d0 = parameters.eta * d0 - alphak1 * parser.evaluateGradientFunction(x1);
            else if constexpr(DefineGrad == "N")
                d0 = parameters.eta * d0  - alphak1 * parser.evaluateGradientDC(x1);
        }
            
        else if constexpr (method == SecondOrderMethod::Adam)
        {
            float alpha = handleDecay<decayType,method>(parameters, k, parser, gradientValues_x0);
            
            auto power_func_vec = [](double power, const std::vector<double>& vec)
            {
                std::vector<double> result(vec.size());
                std::transform(vec.begin(), vec.end(), result.begin(), [power](double num) {
                    return std::pow(num, power);
                });
                return result;
            };

            std::vector<double> grad_norm_x0 = power_func_vec(2, gradientValues_x0);

            m = parameters.beta1 * m + (1 - parameters.beta1) * gradientValues_x0;
            v = parameters.beta2 * v + (1 - parameters.beta2) * grad_norm_x0;
            
            auto power_func = [](double beta1, int k) 
            {
                return std::pow(beta1, k + 1);
            };
            
            std::vector<double> m_hat = (1/(1 - power_func(parameters.beta1, k))) * m;
            std::vector<double> v_hat = (1/(1 - power_func(parameters.beta2, k))) * v;         
            
            for(int i = 0; i < parameters.dim; ++i)
            {   
                x1[i] = x0[i] - alpha * m_hat[i] / (std::sqrt(v_hat[i]) + parameters.epsilon);
            }  
        }

        else 
        {
            std::cout << "Wrong choice of second order method" << std::endl;
            exit(1);
        }

        // Step 3: Compute the norm of the gradient at x0 for the stopping criteria

        double grad_norm_x0 = std::sqrt(std::inner_product(gradientValues_x0.begin(), gradientValues_x0.end(), gradientValues_x0.begin(), 0.0));

        // Step 4: Update the iteration counter

        k++;

        // Step 5: Check the stopping criteria

        if (distance(x1, x0) < parameters.tol_r || grad_norm_x0 < parameters.tol_s) 
        {
            break;
        }
        
        // Step 6: Update xk 

        x0 = x1;

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
    parameters.beta1 = j["beta1"];
    parameters.beta2 = j["beta2"];  
    parameters.epsilon = j["epsilon"];
    parameters.dim = j["dim"];
    parameters.sigma = j["sigma"];
    parameters.tol_r = j["tol_r"];
    parameters.tol_s = j["tol_s"];
    parameters.iter = j["iter"];

    return parameters;
}

void readFunctionAndGradient(std::vector<double>& initial_values, my_Parser& parser, const Parameters& parameters) 
{
    // Open the JSON file
    std::ifstream file("parameters.json");
    if (!file) 
    {
        throw std::runtime_error("Could not open file parameters.json");
    }

    // Parse the JSON file
    nlohmann::json j;
    file >> j;

    // Read the initial values and gradient from the JSON file

    initial_values = j["initial_values"].get<std::vector<double>>();
    std::vector<std::string> gradient(initial_values.size());

    // Set the function 

    parser.setFunction(j["function"]);


    // Check if the gradient has been passed

    if constexpr (DefineGrad == "Y")
    {
        
        gradient = j["gradient"];

        // Check that the sizes match

        if (static_cast<size_t>(parameters.dim) != initial_values.size() || static_cast<size_t>(parameters.dim) != gradient.size()) 
        {
            throw std::runtime_error("Mismatch between problem dimension and size of initial values or gradient");
        }

        // Define Grad

        for (int i = 0; i < parameters.dim; ++i) 
        {
            parser.setGradientFunction(i, gradient[i]);
        }
    }

    else if constexpr (DefineGrad == "N")
    {
        if (static_cast<size_t>(parameters.dim) != initial_values.size()) 
        {
            throw std::runtime_error("Mismatch between problem dimension and size of initial values");
        }
    }
    else 
    {
        std::cout << "Wrong choice of gradient definition" << std::endl;
        exit(1);
    }
    
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
        std::cout << "\n\n-----------------------------------\n";
        std::cout << "Solution Converged at iteration: " << sol.iter << "\n";
        std::cout << "-----------------------------------\n";
    }
    else 
    {
        std::cout << "\n\n-----------------------------------\n";
        std::cout << "The algorithm did not converge...\n(Or maybe you set the number of iterations too low)\nAnyway, here is the last solution found:\n";
        std::cout << "-----------------------------------\n";
    }

        // Print the decay type
        std::cout << "Decay Type: " << decayToString(decay) << "\n";

        // Print the method used
        std::cout << "Method Used: " << methodToString(method) << "\n\n";

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
        std::cout << "With corresponding value: " << sol.minimum << "\n";

        std::cout << "-----------------------------------\n";
}
