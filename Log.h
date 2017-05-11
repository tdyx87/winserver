#ifndef LOG_H__
#define LOG_H__



#include <Windows.h>
#include <assert.h>
#include <stdio.h>

 

 
#define CONTENT_TITLE 32
 



class CLog
{
	
 
public:	
    // constructor
	
    CLog(void);
	
	
    //destructor
	
    ~CLog(void);
	
private:	
    // copy constructor
	
    CLog (const CLog &);
	
	
    //overloaded 
	
    const CLog & operator = (const CLog &);
	
private:
	
    CRITICAL_SECTION  m_Lock;
	
	
    // file descriptor
	
    FILE * m_File;
	
	
    //create time
	
    SYSTEMTIME m_Time;
	
	
    // content title
	
    char szContentTitle[CONTENT_TITLE];
	
	
 
	
	
public:
	
    // get instance
	
    static CLog & GetInstance();
	
	
    // log a formate string
	
    void Logf(const char * lpFormat, ...);
	
	
};

#endif // Log_h__