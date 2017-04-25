#ifndef _TcpListener_H
#define _TcpListener_H

#include <winsock2.h>
#include <vector>
#include <unordered_map>
#include <thread>
#include "PVL.h"
#define DEFAULT_BUFLEN 512
#define MAX_SOCKETS 10


class Connection
{
public:
	static int ID_COUNT;
	SOCKET socket;
	std::string Name;
	int ID;
	bool connected;
	bool ready_to_recieve;
	std::thread t;
	std::string Password;
	std::thread::id thread_id;
	Connection() : ID(ID_COUNT), Name("default"), connected(false), ready_to_recieve(true)
	{
		ID_COUNT++;
	}

};


class TcpListener
{
	bool running = false;
	PCSTR DEFAULT_PORT = "27015";
	SOCKET ListenSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int len;
	struct sockaddr clntAdd;
public:
	//std::vector<Connection*> Connections;
	std::unordered_map<std::string, Connection*> Connections;
	bool isRunning() { return running; }
	void Start();
	void AcceptTcpClient();
	void Listen(Connection* connection);
	void closeConnection(Connection* connection);
	void Stop();
	void SendToAll(trassmision type, std::string& messege);
	void EstablishAccount(Connection* connection);
	void SendTo(trassmision type, std::string& messege, Connection* connection, bool stream = true);
	std::string Receive(Connection* connection, int* type);

};


#endif // !_TcpListener_H