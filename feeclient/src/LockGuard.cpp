//#include "LockGuard.hpp"
#include "feeclient/include/LockGuard.hpp"
#include <pthread.h>
#include <cerrno>

using namespace dcs::fee;

Mutex::Mutex()
{
  fpMutex=new pthread_mutex_t;
  if (fpMutex)
    pthread_mutex_init((pthread_mutex_t*)fpMutex, 0);
}

Mutex::~Mutex()
{
  if (fpMutex) {
    pthread_mutex_destroy((pthread_mutex_t*)fpMutex);
    delete (pthread_mutex_t*)fpMutex;
  }
}

int Mutex::Lock()
{
  if (fpMutex)
    return pthread_mutex_lock((pthread_mutex_t*)fpMutex);

  return -EFAULT;
}

int Mutex::Unlock()
{
  if (fpMutex)
    return pthread_mutex_unlock((pthread_mutex_t*)fpMutex);

  return -EFAULT;
}

 
LockGuard::LockGuard(Mutex& m) 
  : fMutex(m)
{
  fMutex.Lock();
}

LockGuard::~LockGuard() 
{
  fMutex.Unlock();
}

