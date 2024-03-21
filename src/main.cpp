#include "minimum.hpp"

int main(int argc , char *argv[])
{
    Parameters parameters(readParameters());
    
    std::vector<std::pair<double,double>> results;

    // Compute the minimum using the specified decay and method

    Solution sol(ComputeMinimum<decay,method>(parameters,results));

    printSolution(sol);

    // Plot the results (Works only for 2D)

    if( parameters.dim == 2 && argc > 1 && strcmp(argv[1], "plot") == 0)
    {
        plot(results);
    }

    return 0;
}
