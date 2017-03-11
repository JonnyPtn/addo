// crt_popen.c  
/* This program uses _popen and _pclose to receive a
* stream of text from a system process.
*/
#include <Windows.h>
#include <strsafe.h>
#include <string>
#include <locale>
#include <codecvt>
#define BUFSIZE 4096 

std::string GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}

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

    auto pipe = CreateNamedPipe("\\\\.\\pipe\\sudopipe",
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        BUFSIZE,
        BUFSIZE,
        0,
        NULL
    );
    if (pipe == INVALID_HANDLE_VALUE)
        printf(GetLastErrorAsString().c_str());

    // Create the child process. 

    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    BOOL bSuccess = FALSE;

    // Set up members of the PROCESS_INFORMATION structure. 

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure. 
    // This structure specifies the STDIN and STDOUT handles for redirection.

    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = pipe;
    siStartInfo.hStdOutput = pipe;
    siStartInfo.hStdInput = pipe;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Create the child processs
    /*bSuccess = CreateProcess(NULL,
        const_cast<char*>(("pipedcmd.exe " + command).c_str()),     // command line 
        NULL,          // process security attributes 
        NULL,          // primary thread security attributes 
        TRUE,          // handles are inherited 
        0,             // creation flags 
        NULL,          // use parent's environment 
        NULL,          // use parent's current directory 
        &siStartInfo,  // STARTUPINFO pointer
        &piProcInfo);  // receives PROCESS_INFORMATION */

                       //shell execute instead?
                       SHELLEXECUTEINFO shExInfo = { 0 };
                       shExInfo.cbSize = sizeof(shExInfo);
                       shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
                       shExInfo.hwnd = 0;
                       shExInfo.lpVerb = "runas";                // Operation to perform
                       shExInfo.lpFile = "C:\\Users\\Jonny\\Documents\\Visual Studio 2015\\Projects\\Project3\\Debug\\pipedcmd.exe";       // Application to start
                       shExInfo.lpParameters = command.c_str();                  // Additional parameters
                       shExInfo.lpDirectory = 0;
                       shExInfo.nShow = SW_HIDE;
                       shExInfo.hInstApp = 0;

                       if (ShellExecuteEx(&shExInfo))
                       {
                       WaitForSingleObject(shExInfo.hProcess, INFINITE);
                       CloseHandle(shExInfo.hProcess);
                       }

    // Get a handle to an input file for the parent. 
    // This example assumes a plain text file and uses string output to verify data flow. 

    // Write to the pipe that is the standard input for a child process. 
    // Data is written to the pipe's buffers, so it is not necessary to wait
    // until the child process is running before writing data.

    //connect to it
    ConnectNamedPipe(pipe, NULL);

    // Read from pipe that is the standard output for child process. 
    do
    {

        DWORD dwRead, dwWritten;
        CHAR chBuf[BUFSIZE];
        std::memset(chBuf, 0, BUFSIZE);

        //now read from the pipe
        ReadFile(pipe,
            chBuf,
            BUFSIZE, &dwRead, NULL);
        printf(chBuf);
    } while (WaitForSingleObject(piProcInfo.hProcess, 0) == WAIT_TIMEOUT);

    return 0;
}