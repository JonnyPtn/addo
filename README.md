# sudo

This is a program for windows which lets you run terminal commands in the current window with elevated permissions

UAC is still required, unfortunately. Example of it in action [here](https://www.youtube.com/watch?v=O0d89sDNk30)

This is still experimental, so don't expect a bug free experience, and I welcome any feedback.

## Compiling
The project uses cmake, so to build just make sure you have cmake and an msbuild version installed(I've tested with vs2015 and 2017).

Generate the project files using cmake then build the install target

If you use the command line and want to place the files somewhere requiring administrator permissions check out my sudo program [here](https://github.com/JonnyPtn/sudo/)

command line install (presuming you have sudo installed)
`cmake .` followed by `msbuild install.vcxproj`
