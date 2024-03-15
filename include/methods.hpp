
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

#endif // HH_METHODS_HH