#include "stdafx.h"
#include "Windows.h"

#define SERVICE_NAME "OKPayService"
#include "Log.h"


SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hServiceStatusHandle;
void WINAPI service_main(int argc, char** argv); 
void WINAPI ServiceHandler(DWORD fdwControl);
HANDLE task_handle;
HANDLE exitEvent;

extern "C" ServerEntry();

extern "C" ServerExit();

#define writetoLog CLog::GetInstance().Logf 

int uaquit;

DWORD WINAPI srv_core_thread(LPVOID para)
{   
	ServerEntry();
    return 0; 
}   


void WINAPI ServiceHandler(DWORD fdwControl)
{
	switch(fdwControl) 
	{
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
		ServiceStatus.dwWin32ExitCode = 0; 
		ServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
		ServiceStatus.dwCheckPoint    = 0; 
		ServiceStatus.dwWaitHint      = 0;
		writetoLog("开始停止服务");
		SetEvent(exitEvent);
		//add you quit code here
		WaitForSingleObject(task_handle,INFINITE);
		CloseHandle(exitEvent);
		break; 
		default:
			return;	
	};
	if (!SetServiceStatus(hServiceStatusHandle,  &ServiceStatus)) 
	{ 
		DWORD nError = GetLastError();
	} 
}

void WINAPI service_main(int argc, char** argv) 
{       
    ServiceStatus.dwServiceType        = SERVICE_WIN32; 
    ServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    ServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN; 
    ServiceStatus.dwWin32ExitCode      = 0; 
    ServiceStatus.dwServiceSpecificExitCode = 0; 
    ServiceStatus.dwCheckPoint         = 0; 
    ServiceStatus.dwWaitHint           = 0;  
    hServiceStatusHandle = RegisterServiceCtrlHandler(_T(SERVICE_NAME), ServiceHandler); 
    if (hServiceStatusHandle==0) 
    {
		DWORD nError = GetLastError();
    }  
	 
	
    // Initialization complete - report running status 
    ServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
    ServiceStatus.dwCheckPoint         = 0; 
    ServiceStatus.dwWaitHint           = 9000;  
    if(!SetServiceStatus(hServiceStatusHandle, &ServiceStatus)) 
    { 
		DWORD nError = GetLastError();
    } 
	
	exitEvent = CreateEvent(NULL,TRUE,FALSE,"OKPayServiceExitEvent");
	
	task_handle = CreateThread(NULL,NULL,srv_core_thread,NULL,NULL,NULL);
 
	writetoLog("创建新线程");
//	CloseHandle(task_handle);
 
} 

BOOL isInstalled()
{
	BOOL bResult = FALSE;
	
    //打开服务控制管理器
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	
    if (hSCM != NULL)
    {
        //打开服务
        SC_HANDLE hService = ::OpenService(hSCM, SERVICE_NAME, SERVICE_QUERY_CONFIG);
        if (hService != NULL)
        {
            bResult = TRUE;
			writetoLog("服务已经安装过");
            ::CloseServiceHandle(hService);
        }  
        ::CloseServiceHandle(hSCM);
    }
	
    return bResult;
}

void createService()
{
	if (isInstalled())
	{
		writetoLog("不能重复安装");
		return ;
	}
    
    //打开服务控制管理器
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
		writetoLog("打开服务控制管理器失败");
        return;
    }
	
    // Get the executable file path
    TCHAR szFilePath[MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, MAX_PATH);
	
    //创建服务
    SC_HANDLE hService = ::CreateService(hSCM, SERVICE_NAME , SERVICE_NAME ,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS ,SERVICE_AUTO_START , SERVICE_ERROR_NORMAL,
        szFilePath, NULL, NULL, _T(""), NULL, NULL);
	
    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
		writetoLog("创建服务失败");
        return;
    }
	writetoLog("创建服务成功");
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return;
}

void startService()
{
	if (!isInstalled())
	{
		writetoLog("服务没有安装过");
		return ;
	}
	
	
	writetoLog("正在启动服务");
	
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	
    if (hSCM == NULL)
    {
		writetoLog("打开服务控制管理器失败");
        return  ;
    }
	
    SC_HANDLE hService = ::OpenService(hSCM, SERVICE_NAME, SERVICE_START);
	
    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        writetoLog("打开服务失败");
        return  ;
    }
    
	BOOL isSucess = StartService(hService,0,NULL);  
	DWORD err = 0;  
	if (!isSucess)  
    {
		writetoLog("启动服务失败");
	}
	else
	{
		writetoLog("启动服务成功");
	}
    //删除服务

    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
	
    return  ;
}

void stopService()
{
	SC_HANDLE scHandle = OpenSCManager(NULL,NULL,SC_MANAGER_CONNECT);  
    SC_HANDLE hsev = NULL;  
    if(scHandle != NULL)  
    {  
        hsev = OpenService(scHandle,SERVICE_NAME,SERVICE_ALL_ACCESS);  
        SERVICE_STATUS mss;  
        if(hsev != NULL)  
        {  
            QueryServiceStatus(hsev,&mss);  
            if (mss.dwCurrentState == SERVICE_RUNNING)  
            {  
                ControlService(hsev,SERVICE_CONTROL_STOP,&mss);  
                CloseServiceHandle(hsev);  
            }                 
        }  
        CloseServiceHandle(scHandle);  
    }   
}

void deleteService()
{
	if (!isInstalled())
	{
		writetoLog("服务没有安装过");
		return ;
	}
        
	
	writetoLog("正在删除服务");
	
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	
    if (hSCM == NULL)
    {
		writetoLog("打开服务控制管理器失败");
        return  ;
    }
	
    SC_HANDLE hService = ::OpenService(hSCM, SERVICE_NAME, SERVICE_STOP | DELETE);
	
    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        writetoLog("打开服务失败");
        return  ;
    }
    SERVICE_STATUS status;
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);
	
    //删除服务
    BOOL bDelete = ::DeleteService(hService);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
	
    if (bDelete)
	{
		writetoLog("删除服务成功");
		return;
	}
    writetoLog("删除服务失败");
    return  ;
}

void startServiceDispatcher()
{
	SERVICE_TABLE_ENTRY ServiceTable[2];
    ServiceTable[0].lpServiceName = _T(SERVICE_NAME);
    ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)service_main;
    ServiceTable[1].lpServiceName = NULL;
    ServiceTable[1].lpServiceProc = NULL;
    StartServiceCtrlDispatcher(ServiceTable); 
}

//do not change main function
int main (int argc, const char *argv[])
{

/*#if _DEBUG
	
	writetoLog("323324324");
	ServerEntry(); 

	return 0;
#endif
*/
	if(argc==2)
	{
		if(strcmp(argv[1],"install")==0)
		{
			createService();
			startService();
		}
		else if(strcmp(argv[1],"uninstall")==0)
		{
			deleteService();
		}	
	}
	else
	{
		startServiceDispatcher();
	}
    return 0;
}
