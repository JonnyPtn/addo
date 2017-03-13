// crt_popen.c  
/* This program uses _popen and _pclose to receive a
* stream of text from a system process.
*/
#include <Windows.h>
#include <strsafe.h>
#include <string>
#include <locale>
#include <codecvt>
#include <iostream>
#define BUFSIZE 128 

int main(int argc, char* argv[])
{
    //parse input
    std::string command;
    for (int i = 1; i < argc; i++)
    {
        std::string arg(argv[i]);
        if (arg.find(" ") != std::string::npos)
        {
            arg.insert(arg.begin(), '"');
            arg.append("\"");
        }
        command.append(arg);
        command.append(" ");
    }

    //create a named pipe. The shell command we execute with elevated priviledges will write to this
    auto pipe = CreateNamedPipe("\\\\.\\pipe\\sudopipe",
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        BUFSIZE,
        BUFSIZE,
        0,
        NULL
    );


    //shell execute with elevated permissions
    SHELLEXECUTEINFO shExInfo = { 0 };
    shExInfo.cbSize = sizeof(shExInfo);
    shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    shExInfo.hwnd = 0;
    shExInfo.lpVerb = "runas";                // Operation to perform
    shExInfo.lpFile = "C:/sudo/pipedcmd.exe";       // Application to start
    shExInfo.lpParameters = command.c_str();                  // Additional parameters
    shExInfo.lpDirectory = 0;
    shExInfo.nShow = SW_HIDE;
    shExInfo.hInstApp = 0;

    if (ShellExecuteEx(&shExInfo))
    {
        //connect to the pipe
        ConnectNamedPipe(pipe, NULL);
        DWORD dwRead, dwWritten;
        CHAR chBuf[BUFSIZE];
        // Read from the pipe until the process ends
        int status=0;
        do
        {
            dwRead = 0;
            std::memset(chBuf, 0, BUFSIZE);

            //now read from the pipe
            ReadFile(pipe,
                chBuf,
                BUFSIZE, &dwRead, NULL);
            if (dwRead>0)
            {
                std::cout<<std::string(chBuf);
            }
            auto status = WaitForSingleObject(shExInfo.hProcess, 0);
        } while (status == WAIT_TIMEOUT);

        //output anything remaining in the pipe
        do
        {
            std::memset(chBuf, 0, BUFSIZE);
            dwRead = 0;
            ReadFile(pipe,
                chBuf,
                BUFSIZE, &dwRead, NULL);
            if (dwRead>0)
            {
                std::cout<< std::string(chBuf);
            }
        } while (dwRead > 0);
    }
    return 0;
}