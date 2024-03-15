
#ifndef HH_METHODS_HH
#define HH_METHODS_HH

// Define the enums

enum DecayType 
{
    Exponential,    // 0
    Inverse,        // 1
    Armijo          // 2
};

enum SecondOrderMethod 
{
    None,           // 0 
    HeavyBall,      // 1
    Nesterov,       // 2
    Adam            // 3
};

// Choose your decay type

constexpr std::string_view DefineGrad = "N";
constexpr DecayType decay = DecayType::Exponential;
constexpr SecondOrderMethod method = SecondOrderMethod::Adam;


// Define auxiliary functions for the print of the solutions

std::string decayToString(DecayType decay) 
{
    switch (decay) {
        case DecayType::Exponential: return "Exponential";
        case DecayType::Inverse: return "Inverse";
        case DecayType::Armijo: return "Armijo";
        default: return "Unknown";
    }
}

std::string methodToString(SecondOrderMethod method) 
{
    switch (method) {
        case SecondOrderMethod::None: return "None";
        case SecondOrderMethod::HeavyBall: return "HeavyBall";
        case SecondOrderMethod::Nesterov: return "Nesterov";
        case SecondOrderMethod::Adam: return "Adam";
        default: return "Unknown";
    }
}

#endif // HH_METHODS_HH