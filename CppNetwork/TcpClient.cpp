#include "TcpClient.h"
#include "PVL.h"
#define WIN32_LEAN_AND_MEAN
#include <ws2tcpip.h>

using std::string;

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")
//#pragma comment (lib, "AdvApi32.lib")

TcpClient::TcpClient() : connected(false), ConnectSocket(INVALID_SOCKET), result(NULL), ptr(NULL), iResult(0), recvbuflen(DEFAULT_BUFLEN), DEFAULT_PORT("27015"), serveraddress("127.0.0.1")
{
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
}

void TcpClient::Connect(std::string& user_name, std::string& password, std::string& ip, std::string& port)
{
	if (connected)
		return;

	if (user_name == "")
		throw "Invalid username";

	this->user_name = user_name;
	this->password = password;
	this->DEFAULT_PORT = port.c_str();
	this->serveraddress = ip;
	push_new_messege("Validate the parameters and connecting to server..");
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
		throw "WSAStartup failed";

	push_new_messege("Resolve the server address and port");
	push_new_messege(serveraddress.c_str());
	push_new_messege(std::string(DEFAULT_PORT).c_str());
	// Resolve the server address and port
	iResult = getaddrinfo(serveraddress.c_str(), DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		throw "Error";
	}
	push_new_messege("Attempt to connect to an address until one succeeds");
	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET)
		{
			push_new_messege("socket failed with");
			WSACleanup();
			throw "Error";
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) 
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		throw "Error";
	}

	push_new_messege("Authenticating account...");
	connected = true;
	EstablishAccount();
}

void TcpClient::EstablishAccount()
{
	int type = trassmision::invalid;
	Send(ok, user_name);
	Receive(&type);
	if (type != trassmision::ok)
	{
		push_new_messege("Server refused connection: Name already exists");
		return;
	}
	Send(ok, password);
	Receive(&type);
	if (type != trassmision::ok)
	{
		push_new_messege("Server refused connection: Password is incorrect");
		return;
	}

	push_new_messege("You are now connected!");
	listen_thread = std::thread(&TcpClient::Start_Listen, this);
}

void TcpClient::Start_Listen()
{
	int type = trassmision::ok;
	while (type != trassmision::invalid)
	{
		std::string data = Receive(&type);
		switch (type)
		{
		case trassmision::normal:
			push_new_messege(data.c_str());
			break;
		case trassmision::join:
			online.push_back(data);
			push_new_messege(std::string(data + std::string(" has joined the channel!")).c_str());
			break;
		case trassmision::left:
			online.erase(std::find(online.begin(), online.end(), data));
			push_new_messege(std::string(data + std::string(" has left the channel!")).c_str());
			break;
		case trassmision::ok:
			break;
		default:
			push_new_messege("Messege couldnt be proccessed...");
			break;
		}
	}
}

std::string TcpClient::Receive(int* type)
{
	int iResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

	if (iResult <= 0)
	{
		*type = trassmision::invalid;
		push_new_messege("You have been disconnected");
		CloseConnection();
		return std::string("");
	}

	*type = recvbuf[0];
	Send(trassmision::ok, std::string(""));

	std::string data = std::string(recvbuf);
	return data.substr(1, data.length() - 1);

}

void TcpClient::Send(trassmision type, std::string& messege)
{
	if (!connected)
	{
		push_new_messege("You are disconnected");
		return;
	}
	
	std::string outgoing = "0" + messege;
	outgoing[0] = type;
	int iSendResult;
	iSendResult = send(ConnectSocket, outgoing.c_str(), static_cast<int>(outgoing.length() + 1), 0);
	if (iSendResult == SOCKET_ERROR)
		CloseConnection();
}

void TcpClient::CloseConnection()
{
	online.clear();
	if (!connected)
		return;

	connected = false;
	// shutdown the connection since no more data will be sent
	iResult = closesocket(ConnectSocket);
	if (iResult == SOCKET_ERROR) 
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		throw "error";
	}
	WSACleanup();
	

	if (listen_thread.get_id() != std::this_thread::get_id() &&  listen_thread.joinable())
	{
		listen_thread.join();
	}
}