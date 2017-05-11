
#include "SocketClient.h"
#include "ThreadPool.h"
#include "winsock2.h"
#pragma comment(lib,"ws2_32.lib")
SocketClient::SocketClient()
{
	 initSocket();
}

SocketClient::~SocketClient()
{
	uninitSocket();
}

int SocketClient::initSocket()
{
	WSADATA wsa;
	/*初始化socket资源*/
	if (::WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		return - 1; //代表失败
	}
	return 0;
}

int SocketClient::uninitSocket()
{
	::WSACleanup();
	return 0;
}

int SocketClient::connecthost(char* ip,int port)
{
	fd_set rfd;      //描述符集 这个将测试连接是否可用
	struct timeval timeout;  //时间结构体
	FD_ZERO(&rfd);//先清空一个描述符集
	timeout.tv_sec = 5;//秒
	timeout.tv_usec = 0;//一百万分之一秒，微秒
	u_long ul=1;//代表非阻塞
	
	m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	::ioctlsocket(m_socket,FIONBIO,&ul);//设置为非阻塞连接


	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.s_addr = inet_addr(ip);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(port);
	int ret = ::connect(m_socket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	FD_SET(m_socket,&rfd);
	ret = select(0, 0, &rfd, 0, &timeout);
	if(ret<=0)
	{
		return -1;
	}
	ul = 0;
	ioctlsocket(m_socket,FIONBIO,&ul);
	return ret;
}

int SocketClient::send(char* data)
{
	int nNetTimeout=50000;//1秒
	setsockopt(m_socket,SOL_SOCKET, SO_SNDTIMEO, (char *)&nNetTimeout,sizeof(int));
	return	::send(m_socket,data,strlen(data),0);
}

int SocketClient::recv(char *&data)
{
	char buffer[4096]={'\0'}; 
	
	int nNetTimeout=50000;//1秒

	setsockopt(m_socket,SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout,sizeof(int));

    int buflen  = ::recv(m_socket,buffer,sizeof(buffer),0);

	data = buffer;


	return buflen;
}
int SocketClient::clostsocket()
{
	::closesocket(m_socket);

	return 0;
}