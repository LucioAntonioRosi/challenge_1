#ifndef MINIMUM_H
#define MINIMUM_H

#include <iostream>
#include <functional>

// Function that takes two function wrappers, a double, an integer, a float, and a double as inputs
void myFunction(const std::function<void()>& func1, const std::function<void()>& func2, double d, int i, float f, double d2) {
    // Call the first function wrapper
    func1();

    // Call the second function wrapper
    func2();

    // Print the values of the inputs
    std::cout << "Double: " << d << std::endl;
    std::cout << "Integer: " << i << std::endl;
    std::cout << "Float: " << f << std::endl;
    std::cout << "Double 2: " << d2 << std::endl;
}

#endif MINIMUM_H