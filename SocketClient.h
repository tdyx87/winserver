#ifndef _SOCKETCLIENT_H_
#define _SOCKETCLIENT_H_
#include <string>
#include <Winsock2.h>
using namespace std;
class SocketClient
{
public:
	SocketClient();
	int initSocket();
	int uninitSocket();
	int connecthost(char* ip,int port);
	int send(char* data);
	int recv(char *&data);
	int clostsocket();
	~SocketClient();
private:
	SOCKET m_socket;
};
#endif