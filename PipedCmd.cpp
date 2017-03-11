#include <windows.h>
#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#define BUFSIZE 4096

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL)
            result += buffer.data();
    }
    return result;
}

int main(int argc, char* argv[])
{
    //parse input
    std::string command = "cmd /c";
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

    DWORD dwRead, dwWritten;
    HANDLE hStdin, hStdout;
    BOOL bSuccess;

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (
        (hStdout == INVALID_HANDLE_VALUE) ||
        (hStdin == INVALID_HANDLE_VALUE)
        )
        ExitProcess(1);
        
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(_popen(command.c_str(), "r"), _pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL)
            result += buffer.data();
    }

    //bSuccess = WriteFile(hStdout, result.data(), result.length(), &dwWritten, NULL);

    // Open the named pipe
    // Most of these parameters aren't very relevant for pipes.
    HANDLE hpipe;
    do
    {
        hpipe = CreateFile(
            "\\\\.\\pipe\\sudopipe",
            GENERIC_WRITE, // only need read access
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

    } while (hpipe == INVALID_HANDLE_VALUE);
 
    auto  wresult = WriteFile(
        hpipe, // handle to our outbound pipe
        result.data() , // data to send
        result.length(), // length of data to send (bytes)
        &dwWritten, // will store actual amount of data sent
        NULL // not using overlapped IO
    );

    if (!wresult) {
        // look up error code here using GetLastError()
        // system("pause");
    }
    return 0;
}