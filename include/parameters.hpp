#ifndef HPP_PARAMETERS_HPP
#define HPP_PARAMETERS_HPP

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "my_parser.hpp"

using namespace mup;

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

#endif // HPP_PARAMETERS_HPP
