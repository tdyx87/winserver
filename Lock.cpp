

#include "lock.h"

Lock::Lock(Mutex &mutex): m_mutex(mutex), m_locked(true)
{
    m_mutex.Lock();
}

Lock::~Lock(void)
{/*һ��Ҫ�����������н�������Ϊ���ܷ���ʲô��ֻҪ�����뿪�����������ڣ����뿪�����ţ��������������������*/
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