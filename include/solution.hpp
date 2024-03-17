#ifndef HPP_SOLUTION_HPP
#define HPP_SOLUTION_HPP

#include <iostream>
#include <vector>
#include "methods.hpp"


struct Solution
{
    bool converged = false;
    std::vector<double> minimum_coords;
    double minimum;
    int iter;
};

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

        // Tell if gradient was provided by the user 

        std::cout << "Gradient Provided: " << DefineGrad << "\n";

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

#endif // HPP_SOLUTION_HPP