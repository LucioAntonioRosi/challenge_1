/*SMALL HEADER FILE TO PRINT THE SOLUTION 
FOUND BY THE OPTIMIZATION ALGORITHM.*/

#ifndef HPP_SOLUTION_HPP
#define HPP_SOLUTION_HPP

#include <iostream>
#include <vector>
#include "methods.hpp"
#include "gnuplot-iostream.hpp"


struct Solution
{
    bool converged = false;
    std::vector<double> minimum_coords;
    double minimum;
    int iter;
    bool isNaN;
};

void printSolution(const Solution& sol)
{
    // Check if the solution has converged
    
    if (sol.converged && !sol.isNaN) 
    {
        std::cout << "\n\n-----------------------------------\n";
        std::cout << "Solution Converged at iteration: " << sol.iter << "\n";
        std::cout << "-----------------------------------\n";
    }
    else if (!sol.isNaN)
    {
        std::cout << "\n\n-----------------------------------\n";
        std::cout << "The algorithm did not converge...\n(Or maybe you set the number of iterations too low)\nAnyway, here is the last solution found:\n";
        std::cout << "-----------------------------------\n";
    }
    else 
    {
        std::cout << "\n\n-----------------------------------\n";
        std::cout << "The solution diverged at iteration: " << sol.iter << "... Maybe try changing your parameters." << "\n";
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

// Function to plot the results of the optimization algorithm (only for 2D problems)

void plot(const std::vector<std::pair<double,double>>& results)
{
    Gnuplot gp; 

    // Set up the plot

    gp << "set title 'Convergence of points xk'\n";
    gp << "set xlabel 'X'\n";
    gp << "set ylabel 'Y'\n";
    gp << "set grid\n";
    gp << "set term wxt 0\n";
    gp << "plot '-' with linespoints title 'points'\n";

    //Now start sending data

    for (const auto& point : results) {
    gp << point.first << " " << point.second << "\n";
    gp.flush();  
    }

    gp << "e\n";  // End of dataset
}

#endif // HPP_SOLUTION_HPP