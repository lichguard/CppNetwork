#include <iostream>
#include "TcpListener.h"
#include  <thread>
#include <Windows.h>
#include <string>

using namespace std;

/*
BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType)
{
	if (dwCtrlType == CTRL_CLOSE_EVENT)
	{
		return TRUE;
	}

	return FALSE;

}
*/
int main()
{
	TcpListener server;
	try
	{
		cout << "Starting server..." << endl;
		server.Start();
		cout << "Server Running..." << endl;
		
		string exit_str;
		std::cin >> exit_str;
		if (exit_str == "stop")
			server.Stop();
		
		server.Stop();
		
	}
	catch (const char* msg)
	{
		cout << "ERROR: " << msg << endl;
	}

	return 0;
}