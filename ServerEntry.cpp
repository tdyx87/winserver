
#include "threadpool.h"
#include "SocketServer.h"
#include "IniSetting.h"
using namespace std;
  
int OKPay3(void *p)
{
	IniSetting setting("OKPay2.ini");
	char *ip = setting.readIni("Server","SERVER_IP");
	char *port = setting.readIni("Server","SERVER_PORT");
	SocketServer server(ip,atoi(port));
	delete ip;
	delete port;
	server.initSocket();
	server.createServer();
	server.waitConnection();

	return 1;
}

int  OKPay4(void *p)
{
	IniSetting setting("OKPay2.ini");
	char *ip = setting.readIni("Server","Host");
	char *port = setting.readIni("Server","Port");
	SocketServer server(ip,atoi(port));
	delete ip;
	delete port;
	server.initSocket();
	server.createServer();
	server.waitConnection();

	return 1;
}
ThreadPool *g_sTp;
extern "C" ServerEntry() 
{  
    g_sTp = new ThreadPool(2);
	g_sTp->Call(OKPay3);
	g_sTp->Call(OKPay4);
	g_sTp->EndAndWait();
	return 0;
}  

extern "C" ServerExit() 
{  
	g_sTp->End();
	delete g_sTp;
	return 0;
} 
