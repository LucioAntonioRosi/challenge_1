
#ifndef HH_METHODS_HH
#define HH_METHODS_HH

// Define the enums

enum DecayType 
{
    Exponential,
    Inverse,
    Armijo
};

enum SecondOrderMethod 
{
    None,
    HeavyBall,
    Nesterov
};

// Choose your decay type

constexpr std::string_view Numerical_grad = "Y";
constexpr DecayType decay = DecayType::Exponential;
constexpr SecondOrderMethod method = SecondOrderMethod::None;

#endif // HH_METHODS_HH