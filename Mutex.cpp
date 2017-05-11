

#include "Mutex.h"

Mutex::Mutex(void)
{
    InitializeCriticalSection(&m_criticalSection);
}

Mutex::~Mutex(void)
{
    DeleteCriticalSection(&m_criticalSection);//保证对象被析构时候能够删除临界区
}

void Mutex::Lock()
{
     EnterCriticalSection(&m_criticalSection);
}

void Mutex::UnLock()
{
    LeaveCriticalSection(&m_criticalSection);
}
 