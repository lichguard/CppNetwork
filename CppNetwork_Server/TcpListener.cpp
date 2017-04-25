#include "TcpListener.h"

#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <ws2tcpip.h>
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

int Connection::ID_COUNT = 0;



void TcpListener::Start()
{
	int iResult;
	WSADATA wsaData;
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		throw "WSAStartup failed with error";
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	len = sizeof(clntAdd);
	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		WSACleanup();
		throw "getaddrinfo failed with error";
	}


	// Create a SOCKET for connecting to server
	this->ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		freeaddrinfo(result);
		WSACleanup();
		throw "socket failed with error";
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		throw "bind failed with error";
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		closesocket(ListenSocket);
		WSACleanup();
		throw "listen failed with error";

	}

	this->running = true;
	std::thread(&TcpListener::AcceptTcpClient, this).detach();
}


void TcpListener::AcceptTcpClient()
{
	std::cout << "New Thread created" << std::endl;

	Connection* newconnection = new Connection();
	newconnection->thread_id = std::this_thread::get_id();

	std::cout << "Awaiting connection..." << std::endl;
	newconnection->socket = accept(ListenSocket, &clntAdd, &len);
	if (newconnection->socket == INVALID_SOCKET)
	{
		closesocket(ListenSocket);
		WSACleanup();
		std::cout << std::endl << "accept failed with error" << std::endl;
		this->running = false;
		return;
	}


	std::cout << "Establishing new connection..." << std::endl;

	//create new thread and DETCHES IT
	std::thread(&TcpListener::AcceptTcpClient, this).detach();

	newconnection->connected = true;
	EstablishAccount(newconnection);
}

void TcpListener::EstablishAccount(Connection* connection)
{
	int type = trassmision::invalid;
	connection->Name = Receive(connection,&type);

	if (this->Connections.find(connection->Name) != this->Connections.end())
	{
		SendTo(trassmision::ok, std::string(""), connection);
		connection->Name = std::string("temp0000");
		closeConnection(connection);
		return;
	}
	
	SendTo(trassmision::ok, std::string("132"), connection, false);
	

	connection->Password = Receive(connection,&type);
	if (connection->Password != "")
	{
		SendTo(trassmision::normal, std::string("Server refused connection: Wrong password"), connection);
		closeConnection(connection);
		return;
	}


	SendTo(trassmision::ok,std::string("123"), connection,false);

	for (auto db_connection : this->Connections)
	{
		int iResult;
		char recvbuf[DEFAULT_BUFLEN];
		int recvbuflen = DEFAULT_BUFLEN;
		iResult = recv(connection->socket, recvbuf, recvbuflen, 0);

		SendTo(trassmision::join, db_connection.second->Name, connection, false);
	}
	
	SendToAll(trassmision::join,std::string(connection->Name));// +" has joined the channel");


	this->Connections[connection->Name] = connection;
	Listen(connection);
}

void TcpListener::Listen(Connection* connection)
{	
	int type = trassmision::ok;
	while (type != trassmision::invalid)
	{
		std::string data = Receive(connection,&type);
		switch (type)
		{
		case trassmision::normal:
			std::cout << connection->Name << " (" << connection->ID << "): " << data << "    " << std::endl;
			SendToAll(trassmision::normal, std::string(connection->Name) + ": " + data);
			break;
		case trassmision::ok:
			connection->ready_to_recieve = true;
			break;
		default:
			break;
		}
	}
}

std::string TcpListener::Receive(Connection* connection,int* type)
{
	int iResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	iResult = recv(connection->socket, recvbuf, recvbuflen, 0);
	if (iResult <= 0)
	{
		connection->connected = false;
		std::cout << connection->Name << " has disconnected" << std::endl;
		SendToAll(trassmision::left, std::string(connection->Name));
		closeConnection(connection);
		//RETURN IS IMPORTANT
		*type = trassmision::invalid;
		return std::string("");
	}
	*type = recvbuf[0];
	std::string data = std::string(recvbuf);
	return data.substr(1, data.length() - 1);
}

void TcpListener::SendToAll(trassmision type, std::string& messege)
{
	for (auto connection : this->Connections)
		SendTo(type, messege, connection.second);
}

void TcpListener::SendTo(trassmision type, std::string& messege, Connection* connection,bool stream)
{
	std::string outgoing = "0" + messege;
	outgoing[0] = type;
	int iSendResult;
	if (connection->connected)
	{
		connection->ready_to_recieve = false;
		iSendResult = send(connection->socket, outgoing.c_str(), static_cast<int>(outgoing.length() + 1), 0);
		if (iSendResult == SOCKET_ERROR)
		{
			closeConnection(connection);
			throw "send failed with error";
		}
	
	}
	
}

void TcpListener::closeConnection(Connection* connection)
{
	int iResult;
	connection->connected = false;
	std::cout << "Closing Connection, ID: " << connection->ID << ", and destroying thread" << std::endl;
	iResult = closesocket(connection->socket); //shutdown(connection->socket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		WSACleanup();
		throw "shutdown failed with error: ";
	}
	
	std::unordered_map<std::string, Connection*>::iterator it = this->Connections.find(connection->Name);
	delete connection;
	if (it != this->Connections.end())
	this->Connections.erase(it);
}
	
void TcpListener::Stop()
	{
		if (!running)
			return;

		while(this->Connections.size() > 0)
			closeConnection(this->Connections.begin()->second);
	
	
		std::cout << "Stopping Server.." << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		running = false;
	}