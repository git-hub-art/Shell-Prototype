My attempt at constructing a minimal shell implementation.

This shell at the moment supports most commands with the help of the PATH based execution, it also supports a few built in functions, feel free to check out the built in array at the top of the code, Kindly note it does show echo as built in but due to difficulties and complexity of parsing the commands for echo then feeding it into built in, the implementation has been commented out and the echo running in the shell is the bin/echo.

The shell supports redirection of all three kinds, that is input, out and standard error. On of the noted problems is cd by itself, that is without any arguments throws up a command not found error, it does work with arguments. The shell also has a state machine based argument parser to handle quoting, my grip on that part is still quite shaky, it seems to function but there could be bugs that I missed. 

On of my additions is the prompt function, it is a built in function which lets the user change the standard dollar prompt for something else, another function the "default" gives us the full current directory path prompts, the default has not been added to the builtin array at the time of me writing this. Various sections have been commented out, those were my initial implementations which I ditched as the combined programme kept getting more and more complicated.



Overall the code organization is all over the place, mostly written in patches, I was mostly experimenting and trying to get it to work instead of aiming for a properly structured shell, I plan on trying to implement pipes in this one, although I will try to write a properly organized code to better imitate the shell once I conclude my experiments on this one.



The shell also supports command history with arrow keys and general arrow key movement with the help of readline(), make sure to compile with the ***-lreadline*** (link readline) flag to ensure proper linkage. It was an impulsive last moment addition once I discovered I could not navigate with arrow keys inside the shell while I was trying to use this, the earlier method employed was fget(), I did ended up removing instead of commenting it out like I did with my other approaches. 















