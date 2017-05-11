

#include "lock.h"

Lock::Lock(Mutex &mutex): m_mutex(mutex), m_locked(true)
{
    m_mutex.Lock();
}

Lock::~Lock(void)
{/*一定要在析构函数中解锁，因为不管发生什么，只要对象离开他的生命周期（即离开大括号），都会调用其析构函数*/
     m_mutex.UnLock();
}

void Lock::SetUnlock()
{
     m_locked = false;
}

Lock::operator bool() const
{
    return m_locked;
}