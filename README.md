# challenge_1

> This is the first challenge yy 24/25 for the PACS course. Since it is for the PACS course, I assume you have
> set the environment in the directory **"pacs-example"**. If not, follow the instructions provided by prof. 
> Formaggia in the Extra to be able to use **"MuParserX"** and **".json"**.

## How to **MAKE**

First, go in the **Makefile** and change **PACS_ROOT** to the working directory where the examples reside.
Then, if you have already installed the Extras, copy the following line to **make** and **run** the executable.
> ``` bash
>     make; ./main
> ```
If you only want to change the parameters in **parameters.json** you can simply change them and run again
> ``` bash
>     ./main
> ```
On the other hand, if you want to change the Methods used (you can change between [Exponential,Inverse,Armijo] for the decay rate, [None,HeavyBall,Nesterov,Adam] for the "Second order Methods" and between ["Y","N"] if you do not want to define your gradient by yourself) you first go to **methods.hpp** in the **include** directory, make the changes and then type in your command line
> ``` bash
>     make distclean; make; ./main
> ```

## Warnings!!

- Do not use the **Armijo** rule with any second order methods, as they may not work well together for theoretical reasons!
- When you set the parameters, be careful to define the right number of **dim** (You must be coherent)!

 
## Parameters

> Here is a brief explanation of the parameters used:
> - **alpha0** : initial learning rate used for all the decay rates (default value = 0.1) 
> - **mu** : normalization constant used in Exponential and Inverse decay (defalut value = 0.2)
> - **eta** : memory parameter used in HeavyBall and Nesterov (default value = 0.9) 
> - **sigma** : helper parameter (between 0 and 0.5) used for the Armijo rule (default value = 0.2)
> - **beta 1** and **beta 2** : memory parameters used for the Adam rule (default values = 0.9 and 0.999 respectively)
> - **epsilon** : helper parameter for Adam rule (default value = 1e-8)
> - **iter** : maximum number of iterations (default value = 10000)
> - **tol_r** and **tol_s** : tolerances for the control on the residual and step_length (default values = 1e-6)
> - **dim** : number of variables (e.g. 2 if you are in R2)
> - **function** : a string that defines your function (e.g. "x0 * x1 + sin(exp(x0))" or "x0 * x1 + 4 * x0 ^ 4 + x1 ^ 2 + 3*x0")
> - **gradient** : an array of strings that defines your gradient (e.g. ["x1 + 16 * x0 ^ 3 + 3", "x0 + 2 * x1"])
> - **initial_values** : an array of the starting point (e.g. [0.0, 0.0]) 
