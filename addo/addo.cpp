////////////////////////////////////////////////////////////////////////////////////////////////////
///						
///											addo
///	         						  Administrator do
///									
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <string>
#include <array>
#include <memory>
#include <stdio.h>
#include <iostream>
#include <codecvt>

#define BUFSIZE 128 

std::wstring myPath() {
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	return std::wstring(buffer);
}


int runpiped(int argc, wchar_t* argv[])
{
	//parse the command first
	std::wstring command = L"cmd /c";
	for (int i = 2; i < argc; i++) // magic 2 because we ignore "-piped" parameter
	{
		std::wstring arg(argv[i]);
		if (arg.find(L" ") != std::wstring::npos)
		{
			//parameter has white space, wrap in speech marks
			arg.insert(arg.begin(), '"');
			arg.append(L"\"");
		}
		command.append(arg);
		command.append(L" ");
	}

	// Open the named pipe
	// Most of these parameters aren't very relevant for pipes.
	HANDLE hpipe;
	do
	{
		hpipe = CreateFile(
			L"\\\\.\\pipe\\addopipe",
			GENERIC_WRITE, // only need write access
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

	} while (hpipe == INVALID_HANDLE_VALUE);

	DWORD dwWritten;

	// I hate this
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	std::string converted_str = converter.to_bytes(command);

	std::array<char, 128> buffer;
	std::shared_ptr<FILE> pipe(_popen(converted_str.c_str(), "r"), _pclose);
	if (!pipe) throw std::runtime_error("popen() failed!");
	while (!feof(pipe.get()))
	{
		if (fgets(buffer.data(), 128, pipe.get()) != NULL)
		{
			std::cout << buffer.data();
			auto  wresult = WriteFile(
				hpipe, // handle to our outbound pipe
				buffer.data(), // data to send
				DWORD(buffer.size()), // length of data to send (bytes)
				&dwWritten, // will store actual amount of data sent
				NULL // not using overlapped IO
			);
		}
	}
	return 0;
}

int wmain(int argc, wchar_t* argv[])
{
	// Make sure we've got something first
	if (argc <= 1)
	{
		std::cout << "Administrator do what?";
		return 0;
	}

	// Parse input
	std::wstring command;

	// If the first parameter is -piped, then we should run piped
	if (std::wstring(argv[1]) == L"-piped")
	{
		return runpiped(argc, argv);
	}
	// Otherwise, we are the pipee
	else
	{
		command.append(L"-piped ");
	}
	for (int i = 1; i < argc; i++)
	{
		std::wstring arg(argv[i]);
		if (arg.find(L" ") != std::wstring::npos)
		{
			arg.insert(arg.begin(), '"');
			arg.append(L"\"");
		}
		command.append(arg);
		command.append(L" ");
	}

	// Create a named pipe. The shell command we execute with elevated priviledges will write to this
	auto pipe = CreateNamedPipe(L"\\\\.\\pipe\\addopipe",
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		BUFSIZE,
		BUFSIZE,
		0,
		NULL
	);

	auto path = myPath();

	// Shell execute with elevated permissions
	SHELLEXECUTEINFO shExInfo = { 0 };
	shExInfo.cbSize = sizeof(shExInfo);
	shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shExInfo.hwnd = 0;
	shExInfo.lpVerb = L"runas";					// Operation to perform
	shExInfo.lpFile = path.c_str();				// Application to start
	shExInfo.lpParameters = command.c_str();	// Additional parameters
	shExInfo.lpDirectory = 0;
	shExInfo.nShow = SW_HIDE;
	shExInfo.hInstApp = 0;

	if (ShellExecuteEx(&shExInfo))
	{
		// Connect to the pipe
		ConnectNamedPipe(pipe, NULL);
		DWORD dwRead;
		CHAR chBuf[BUFSIZE];
		// Read from the pipe until the process ends
		int status = 0;
		do
		{
			dwRead = 0;
			std::memset(chBuf, 0, BUFSIZE);

			// Now read from the pipe
			ReadFile(pipe,
				chBuf,
				BUFSIZE, &dwRead, NULL);
			if (dwRead>0)
			{
				std::cout << std::string(chBuf);
			}
			auto status = WaitForSingleObject(shExInfo.hProcess, 0);
		} while (status == WAIT_TIMEOUT);

		// Output anything remaining in the pipe
		do
		{
			std::memset(chBuf, 0, BUFSIZE);
			dwRead = 0;
			ReadFile(pipe,
				chBuf,
				BUFSIZE, &dwRead, NULL);
			if (dwRead>0)
			{
				std::cout << std::string(chBuf);
			}
		} while (dwRead > 0);
	}
	return 0;
}