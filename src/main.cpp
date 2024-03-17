#include "minimum.hpp"

int main()
{
    Parameters parameters(readParameters("parameters.json"));

    // Compute the minimum using the specified decay and method
    
    Solution sol(ComputeMinimum<decay,method>(parameters));

    printSolution(sol);

    return 0;
}
