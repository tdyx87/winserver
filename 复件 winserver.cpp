#include"stdafx.h"  //������ͷ�ļ�
#define LOGFILE "log.txt" 

extern "C" ServerEntry();
extern "C" ServerExit();
//����ȫ�ֺ�������
void Init();
BOOL IsInstalled();
BOOL Install();
BOOL Uninstall();
void LogEvent(LPCTSTR pszFormat, ...);
void WINAPI ServiceMain();
void WINAPI ServiceStrl(DWORD dwOpcode);
int WriteToLog(char* str); 
TCHAR szServiceName[] = _T("OKPayService");
BOOL bInstall;
SERVICE_STATUS_HANDLE hServiceStatus;
SERVICE_STATUS status;
DWORD dwThreadID;
 
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    Init();
 
    dwThreadID = ::GetCurrentThreadId();
 
    SERVICE_TABLE_ENTRY st[] =
    {
        { szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };
 
    if (stricmp(lpCmdLine, "/install") == 0)
        {
           Install();
        }
    else if (stricmp(lpCmdLine, "/uninstall") == 0)
        {
            Uninstall();
        }
    else
        {
         if (!::StartServiceCtrlDispatcher(st))
        {
            LogEvent(_T("Register Service Main Function Error!"));
        }
      }
 
    return 0;
}
//*********************************************************
//Functiopn:            Init
//Description:          ��ʼ��
//Calls:                main       
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
void Init()
{
    hServiceStatus = NULL;
    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS;
    status.dwCurrentState = SERVICE_START_PENDING;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    status.dwWin32ExitCode = 0;
    status.dwServiceSpecificExitCode = 0;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;
}
 
//*********************************************************
//Functiopn:            ServiceMain
//Description:          ��������������������п��ƶԷ�����Ƶ�ע��
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
void WINAPI ServiceMain()
{
	WriteToLog("service main is running\n");
    // Register the control request handler
    status.dwCurrentState = SERVICE_START_PENDING;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
 
    //ע��������
    hServiceStatus = RegisterServiceCtrlHandler(szServiceName, ServiceStrl);
    if (hServiceStatus == NULL)
    {
        LogEvent(_T("Handler not installed"));
        return;
    }
    SetServiceStatus(hServiceStatus, &status);
 
    status.dwWin32ExitCode = S_OK;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;
    status.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hServiceStatus, &status);
 
    //ģ���������С�Ӧ��ʱ����Ҫ������ڴ˼���
        //���ڴ�д�Ϸ�����Ҫִ�еĴ��룬һ��Ϊ��ѭ��


	//ServerEntry();

    while(1)
    {
           //ѭ����ʲô
			
			WriteToLog(" service main is running\n");
			Sleep(100);

     }   
	 
    status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(hServiceStatus, &status);
    OutputDebugString(_T("Service stopped"));
}
 
//*********************************************************
//Functiopn:            ServiceStrl
//Description:          �������������������ʵ�ֶԷ���Ŀ��ƣ�
//                      ���ڷ����������ֹͣ����������ʱ���������д˴�����
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:                dwOpcode�����Ʒ����״̬
//Output:
//Return:
//Others:
//History:
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
void WINAPI ServiceStrl(DWORD dwOpcode)
{
    switch (dwOpcode)
    {
    case SERVICE_CONTROL_STOP:
		WriteToLog("ֹͣ");
        status.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(hServiceStatus, &status);
		ServerExit();
        PostThreadMessage(dwThreadID, WM_CLOSE, 0, 0);
        break;
    case SERVICE_CONTROL_PAUSE:
		WriteToLog("��ͣ");
        break;
    case SERVICE_CONTROL_CONTINUE:
		WriteToLog("����");
        break;
    case SERVICE_CONTROL_INTERROGATE:
		WriteToLog("��ͣ");
        break;
    case SERVICE_CONTROL_SHUTDOWN:
		WriteToLog("�ر���ͣ");
        break;
    default:
        LogEvent(_T("Bad service request"));
        OutputDebugString(_T("Bad service request"));
    }
}
//*********************************************************
//Functiopn:            IsInstalled
//Description:          �жϷ����Ƿ��Ѿ�����װ
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
BOOL IsInstalled()
{
    BOOL bResult = FALSE;
 
    //�򿪷�����ƹ�����
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
 
    if (hSCM != NULL)
    {
        //�򿪷���
        SC_HANDLE hService = ::OpenService(hSCM, szServiceName, SERVICE_QUERY_CONFIG);
        if (hService != NULL)
        {
            bResult = TRUE;

			WriteToLog(" service Installed\n");

            ::CloseServiceHandle(hService);
        }  
        ::CloseServiceHandle(hSCM);
    }


    return bResult;
}
 
//*********************************************************
//Functiopn:            Install
//Description:          ��װ������
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
BOOL Install()
{
    if (IsInstalled())
        return TRUE;
 

	WriteToLog(" service Install \n");

    //�򿪷�����ƹ�����
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
        //MessageBox(NULL, _T("Couldn't open service manager"), szServiceName, MB_OK);
        return FALSE;
    }
 
    // Get the executable file path
    TCHAR szFilePath[MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, MAX_PATH);
 
    //��������
    SC_HANDLE hService = ::CreateService(hSCM, szServiceName, szServiceName,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS ,SERVICE_AUTO_START , SERVICE_ERROR_NORMAL,
        szFilePath, NULL, NULL, _T(""), NULL, NULL);
 
    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
       // MessageBox(NULL, _T("Couldn't create service"), szServiceName, MB_OK);
        return FALSE;
    }
 
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return TRUE;
}
 
//*********************************************************
//Functiopn:            Uninstall
//Description:          ɾ��������
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
BOOL Uninstall()
{
    if (!IsInstalled())
        return TRUE;
 

	WriteToLog(" service Uninstall \n");

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
 
    if (hSCM == NULL)
    {
       // MessageBox(NULL, _T("Couldn't open service manager"), szServiceName, MB_OK);
        return FALSE;
    }
 
    SC_HANDLE hService = ::OpenService(hSCM, szServiceName, SERVICE_STOP | DELETE);
 
    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        //MessageBox(NULL, _T("Couldn't open service"), szServiceName, MB_OK);
        return FALSE;
    }
    SERVICE_STATUS status;
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);
 
    //ɾ������
    BOOL bDelete = ::DeleteService(hService);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
 
    if (bDelete)
        return TRUE;
 
    LogEvent(_T("Service could not be deleted"));
    return FALSE;
}
 
//*********************************************************
//Functiopn:            LogEvent
//Description:          ��¼�����¼�
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
void LogEvent(LPCTSTR pFormat, ...)
{
    TCHAR    chMsg[256];
    HANDLE  hEventSource;
    LPTSTR  lpszStrings[1];
    va_list pArg;
 
    va_start(pArg, pFormat);
    _vstprintf(chMsg, pFormat, pArg);
    va_end(pArg);
 
    lpszStrings[0] = chMsg;
     
    hEventSource = RegisterEventSource(NULL, szServiceName);
    if (hEventSource != NULL)
    {
        ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCTSTR*) &lpszStrings[0], NULL);
        DeregisterEventSource(hEventSource);
    }
}
 
int WriteToLog(char* str)
{
	FILE* log;
	log = fopen(LOGFILE, "a+");
	if (log == NULL)
		return -1;
	fprintf(log, "%s\n", str);
	fclose(log);
	return 0;
}