#ifndef _TcpClient_H
#define _TcpClient_H

#include <winsock2.h>
#define DEFAULT_BUFLEN 512
#include <vector>
#include <string>
#include <thread>
#include "PVL.h"

extern void push_new_messege(const char *new_messege);
class TcpClient
{

	WSADATA wsaData;
	SOCKET ConnectSocket;
	struct addrinfo *result;
	struct addrinfo *ptr;
	struct addrinfo hints;
	int iResult;
	int recvbuflen;
	PCSTR DEFAULT_PORT;
	std::string serveraddress;
	std::thread listen_thread;
	//UI* UI_Parent;
	bool connected;
	std::string user_name;
	std::string password;

public:
	std::vector<std::string> online;
	TcpClient();
	bool isConnected() { return connected; }
	void Connect(std::string& user_name,std::string& password,std::string& ip, std::string& port);
	void Start_Listen();
	void Send(trassmision type, std::string& trans);
	void CloseConnection();
	void EstablishAccount();
	std::string Receive(int* type);
};
#endif // !_TcpClient_H