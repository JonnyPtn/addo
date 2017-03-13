# sudo

This is a program for windows which lets you run terminal commands in the current window with elevated permissions

Currently just a prototype, I wouldn't recommend using it at all, if you do, you're on your own

It works by abusing some windows API stuff. You need to compile two programs (one is actual sudo, one is pipedcmd). sudo executes pipedcmd with elevated priviledges, passing on the command you gave sudo. Pipedcmd then exectute the command and redirects the output back to the sudo process.

UAC is still required, unfortunately. Example of it in action [here](https://www.youtube.com/watch?v=O0d89sDNk30)
