
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
	/*��ʼ��socket��Դ*/
	if (::WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		return - 1; //����ʧ��
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
	fd_set rfd;      //�������� ��������������Ƿ����
	struct timeval timeout;  //ʱ��ṹ��
	FD_ZERO(&rfd);//�����һ����������
	timeout.tv_sec = 5;//��
	timeout.tv_usec = 0;//һ�����֮һ�룬΢��
	u_long ul=1;//���������
	
	m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	::ioctlsocket(m_socket,FIONBIO,&ul);//����Ϊ����������


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
	int nNetTimeout=50000;//1��
	setsockopt(m_socket,SOL_SOCKET, SO_SNDTIMEO, (char *)&nNetTimeout,sizeof(int));
	return	::send(m_socket,data,strlen(data),0);
}

int SocketClient::recv(char *&data)
{
	char buffer[4096]={'\0'}; 
	
	int nNetTimeout=50000;//1��

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