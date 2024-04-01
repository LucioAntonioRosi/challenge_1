/* THIS IS THE MAIN HEADER, WHERE THE FUNCTION TO COMPUTE THE MINIMUM IS DEFINED. 
   IT USES THE PARSER CLASS TO EVALUATE THE FUNCTION AND ITS GRADIENT. IT ALSO USES 
   THE PARAMETERS CLASS TO READ THE PARAMETERS FROM THE JSON FILE. HERE IT IS ALSO
   DEFINED THE FUNCTION THAT HANDLES THE DECAY OF THE PARAMETER ALPHA.*/

#ifndef HPP_MINIMUM_HPP
#define HPP_MINIMUM_HPP

#include <cmath>
#include "solution.hpp"
#include "parameters.hpp"
#include <limits>


// Define the operators
//@note If you define a free function in a header file you should mark it as inline!!
//inline double distance(const std::vector<double>& x1, const std::vector<double>& x0) 
// Otherwise you have a linking error if two translation units include the header file!!!
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


// Define the function to handle the decay of the parameter alpha at every iteration

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

// Define the function to compute the minimum

template<DecayType decayType, SecondOrderMethod method>
Solution ComputeMinimum (const Parameters& parameters, std::vector<std::pair<double,double>> &results)
{
    if(parameters.dim == 2)
        results.reserve(parameters.iter);

    my_Parser parser(parameters.dim);

    std::vector<double> x0;

    readFunctionAndGradient(x0, parser, parameters);

    // Set the values of the variables
    
    parser.setValues(x0);

    // Define the different variables for the algorithm
    
    int k = 0;
    bool isnan = false;

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

        if (parameters.dim == 2)
            results.push_back(std::make_pair(x0[0], x0[1]));

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

        // Step 5: Check the stopping criteria (and if i got nan) 

        for (int i = 0; i < parameters.dim ; ++i)
        { 
            //@note Nice try. Yet if can be simplified.We have std::isfinite that does this job for you
            // if(!std::isfinite(x[i]) or ....)
            if ( x1[i] == std::numeric_limits<double>::quiet_NaN() || x1[i] == -std::numeric_limits<double>::quiet_NaN()
            || x1[i] == std::numeric_limits<double>::infinity() || x1[i] == -std::numeric_limits<double>::infinity())
            {
                isnan = true;      
            }
        }
        

        if (distance(x1, x0) < parameters.tol_r || grad_norm_x0 < parameters.tol_s || isnan) 
        {
            break;
        }
        
        // Step 6: Update xk 

        x0 = x1;

    }
    
    Solution solution;

    solution.isNaN = isnan;

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

#endif // HPP_MINIMUM_HPP