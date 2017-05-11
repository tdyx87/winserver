#ifndef _SOCKETSERVER_H_
#define _SOCKETSERVER_H_
#include <string>
#include <Winsock2.h>
#include "ThreadPool.h"
using namespace std;
class SocketServer
{
public:
	SocketServer(char*  ip,int port);
	int initSocket();
	int uninitSocket();
	~SocketServer();
	bool createServer();
	void waitConnection();
	char *getIP();
private:
    char* m_ip;
	int m_port;
	SOCKET m_socket; 
};
#endif