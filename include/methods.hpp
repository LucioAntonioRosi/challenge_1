/* THIS FILE CONTAINS THE VARIABLES 
    THAT YOU CAN CHANGE TO TEST THE CODE.
    SEE BELOW FOR MORE DETAILS.*/

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
    Nesterov,       
    Adam            
};


//################################################################################
//#                                                                              #                      
//#             BELOW HERE ARE THE VARIABLES THAT YOU CAN CHANGE!!               #
//#                                                                              #
//################################################################################



constexpr std::string_view DefineGrad = "Y";
constexpr DecayType decay = DecayType::Inverse;
constexpr SecondOrderMethod method = SecondOrderMethod::None;



//################################################################################
//#                                                                              #  
//#             ABOVE HERE ARE THE VARIABLES THAT YOU CAN CHANGE!!               #
//#                                                                              #
//################################################################################






// Define auxiliary functions used only in the solution header file 

std::string decayToString(const DecayType decay)
{
    switch (decay) {
        case DecayType::Exponential: return "Exponential";
        case DecayType::Inverse: return "Inverse";
        case DecayType::Armijo: return "Armijo";
        default: return "Unknown";
    }
}

std::string methodToString(const SecondOrderMethod method)
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
