#include "Log.h"
#include <stdarg.h>
 
 
//************************************
// Method:    CLog
// FullName:  CLog::CLog
// Access:    private 
// Returns:   
// Qualifier:
// Parameter: void
//************************************


#include "IniSetting.h"
 

CLog::CLog(void)
{
    m_File = NULL;
    ::InitializeCriticalSection(&m_Lock);
}
 
 
//************************************
// Method:    ~CLog
// FullName:  CLog::~CLog
// Access:    private 
// Returns:   
// Qualifier:
// Parameter: void
//************************************
 
 
CLog::~CLog()
{
    ::DeleteCriticalSection(&m_Lock);
}
 
 
//************************************
// Method:    GetInstance
// FullName:  CLog::GetInstance
// Access:    public 
// Returns:   CLog &
// Qualifier:
//************************************
 
CLog & CLog::GetInstance()
{
    static CLog m_Log;
    return m_Log;
}
 
 
//************************************
// Method:    Logf
// FullName:  CLog::Logf
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: const char * lpFormat
// Parameter: ...
//************************************



void CLog::Logf(const char * lpFormat, ... )
{   
    __try
    {

        //enter the lock
        ::EnterCriticalSection(&m_Lock);
		
		

		char szAppPath[MAX_PATH]; 
		GetModuleFileName(NULL, szAppPath, MAX_PATH);
		(strrchr(szAppPath, '\\'))[0] = 0; 
		
		char *tmp = strcat(szAppPath,"\\log.txt");

		FILE *f = fopen(tmp,"a+");
		
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		sprintf(szContentTitle, "%02d:%02d:%02d %02d:%02d:%02d:%d->", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);

		
		fprintf(f,szContentTitle);
		
		va_list args;
        va_start(args, lpFormat);
        vfprintf(f, lpFormat, args);
        va_end(args);
		
		fprintf(f,"\n");


		fclose(f);

 
 
        return;
    }
    __finally
    {
        ::LeaveCriticalSection(&m_Lock);
    }
}
  
