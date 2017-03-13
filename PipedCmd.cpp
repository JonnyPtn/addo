////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file   PipedCmd.cpp.
///
/// \brief  This is the process which runs with elevated priviledges.
///         Basically just executes the command and pipes the output back to the sudo process
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <string>
#include <array>
#include <memory>
#include <stdio.h>

#define BUFSIZE 4096

int main(int argc, char* argv[])
{
    //parse the command first
    std::string command = "cmd /c";
    for (int i = 1; i < argc; i++)
    {
        std::string arg(argv[i]);
        if (arg.find(" ") != std::string::npos)
        {
            //parameter has white space, wrap in speech marks
            arg.insert(arg.begin(), '"');
            arg.append("\"");
        }
        command.append(arg);
        command.append(" ");
    }

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

    DWORD dwRead, dwWritten;
    HANDLE hStdin, hStdout;
    BOOL bSuccess;

        
    std::array<char, 64> buffer;
    std::shared_ptr<FILE> pipe(_popen(command.c_str(), "r"), _pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get()))
    {
        if (fgets(buffer.data(), 64, pipe.get()) != NULL)
        {

            auto  wresult = WriteFile(
                hpipe, // handle to our outbound pipe
                buffer.data(), // data to send
                buffer.size(), // length of data to send (bytes)
                &dwWritten, // will store actual amount of data sent
                NULL // not using overlapped IO
            );
        }
    }
    return 0;
}