
#include "SocketServer.h"
#include "ThreadPool.h"
#include "SocketClient.h"
#include "mutex.h"
#include "lock.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;
#include "Log.h"
#define writetoLog CLog::GetInstance().Logf 

ThreadPool *threadpool = 0;

typedef struct stSocketItem
{
	SOCKET socket;
	int port;
	char *ip;
}stSocketItem;


SocketServer::SocketServer(char*  ip,int port)
{
	m_ip = new char[strlen(ip)+1];
	memset(m_ip,'\0',strlen(ip)+1);
	memcpy(m_ip,ip,strlen(ip));
	m_port = port; 
}

SocketServer::~SocketServer()
{
  ::closesocket(m_socket);
  delete m_ip;
  m_ip = NULL;
}

int SocketServer::initSocket()
{
	WSADATA wsa;
	/*初始化socket资源*/
	if (::WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		return - 1; //代表失败
	}
	return 0;
}

int SocketServer::uninitSocket()
{
	::WSACleanup();

	return 0;
}

bool SocketServer::createServer()
{

	m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.s_addr = INADDR_ANY;  ;//= inet_addr(m_ip);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(m_port);

	int ret = ::bind(m_socket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

	if(ret !=0)
	{
		perror("bind error");
		return false;
	}

    ret = ::listen(m_socket,128);

	if(ret != 0)
	{
		perror("listen error");
		return false;
	}
	return false;
}

bool GetAddressBySocket(SOCKET m_socket,SOCKADDR_IN &m_address)
{
    memset(&m_address, 0, sizeof(m_address));
    int nAddrLen = sizeof(m_address);
  
    //根据套接字获取地址信息
    if(::getpeername(m_socket, (SOCKADDR*)&m_address, &nAddrLen) != 0)
    {
        printf("Get IP address by socket failed!n");
        return false;
    }
      
    //读取IP和Port

	 
	printf("IP:%s,Port:%d\n",::inet_ntoa(m_address.sin_addr),ntohs(m_address.sin_port));

    return true;
}
 

int dealClient(void *p)
{
	writetoLog("dealClient");
	Mutex mutex1;
	synchronized(mutex1)
	{
		stSocketItem *socketItem = (stSocketItem*)p;
		SOCKET socket = socketItem->socket;
		int port = socketItem->port;
		char *ip = socketItem->ip;
		writetoLog(ip);
		if(socketItem)
		{
			delete socketItem;
			socketItem=0;
		}
		
		SOCKADDR_IN addr;
		GetAddressBySocket(socket,addr);
		char buffer[4096]={'\0'};
		int nNetTimeout=50000;//60秒
		setsockopt(socket,SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout,sizeof(int));
		::recv(socket,buffer,4096,0);
		writetoLog(buffer);
		SocketClient client;
		int ret  = 0;
		ret = client.connecthost(ip,port);
		if(ret == -1)
		{
			client.clostsocket();
			::closesocket(socket);
			return 0;
		}
		writetoLog("连接服务器");
		ret = client.send(buffer);
		if(ret < 0)
		{
			client.clostsocket();
			::closesocket(socket);
			return 0;
		}
		
		writetoLog("发送数据");
		char * data;
		int len;
		ret = client.recv(data);
		len = ret;
		writetoLog("接收数据");
		writetoLog((char*)(data));
		printf("服务器返回receive: %s\n",data);
		ret = client.clostsocket();
		::send(socket,data,len,0);
		::closesocket(socket);
	};
	return 0;
	
}

char *SocketServer::getIP()
{
	return m_ip;
}



void SocketServer::waitConnection()
{
	//循环 
	HANDLE exitEvent=OpenEvent(EVENT_ALL_ACCESS,FALSE,"OKPayServiceExitEvent");
	while(true)  
    {   
		if(WaitForSingleObject(exitEvent,1)==WAIT_OBJECT_0)
		{
			break;
		}
		
		int maxfdp=NULL;  
		struct fd_set fds;  
		struct timeval timeOut={0,1};  
		FD_ZERO(&fds);  
		FD_SET(m_socket,&fds);  

		if (select(m_socket+1,&fds,NULL,NULL,&timeOut))  
		{         
			SOCKET	connfd = accept(m_socket, NULL,0);  
			if (INVALID_SOCKET == connfd)  
			{   
				continue;  
			}  
			
			if(threadpool==0) threadpool = new ThreadPool(20);
			stSocketItem *socketItem = new stSocketItem();
			socketItem->socket=connfd;
			socketItem->port=m_port;
			socketItem->ip = m_ip;
			threadpool->Call(dealClient,socketItem);

		}
    }  
}