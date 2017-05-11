#ifndef __MUTEX_H__
#define __MUTEX_H__
#include "windows.h"
#define synchronized(M)  for(Lock M##_lock = M; M##_lock; M##_lock.SetUnlock())

class Mutex
{
public:
    Mutex(void);
    ~Mutex(void);
    void Lock();
    void UnLock();
private:
    CRITICAL_SECTION m_criticalSection;
};
 
#endif